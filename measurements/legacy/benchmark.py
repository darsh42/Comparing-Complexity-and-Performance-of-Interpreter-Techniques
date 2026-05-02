import re
import os
import sys
import csv
import signal
import lizard
import pathlib
import subprocess

program = "daxpy"

METRICS = {
    "task-clock": "time_ms",
    "cpu_core/instructions/": "instr_count",
    "cpu_core/cycles/": "cycles",
    "Retiring": "retiring_pct",
    "Frontend Bound": "frontend_pct",
    "Backend Bound": "backend_pct",
    "Bad Speculation": "bad_spec_pct",
    "Branch Mispredict": "br_mispredict_pct",
    "Memory Bound": "mem_bound_pct",
    "CPUs utilized": "cpu_utilization"
}

RUNS = "100"
CORE = "0"
BUILD     = pathlib.Path("./../build")
RESULTS   = pathlib.Path("./../build/measurements")

N_INSTRUCTIONS = 64

W_DISPATCH = 0.70
W_DATA     = 0.29
W_NLOC     = 0.01

BENCHMARKS= {
    "daxpy":         BUILD / "samples/daxpy",
    "linked_list":   BUILD / "samples/linked_list",
    "quicksort":     BUILD / "samples/quicksort",
    "binary_search": BUILD / "samples/binary_search",
    "crc":           BUILD / "samples/crc",
    "Integer Sort":  BUILD / "samples/bin/is.W.x",
}
TECHNIQUES = {
    "Switch-Case":          1,
    "Token-Threading":      2,
    "Tail-Calling":         3,
    "Computed-Goto":        4,
    "Super Instructions":   5,
    "Block Decoding":       6,
    "Stack Caching":        7,
    "Context Threading":    8,
    "Register Allocation":  9,
    "Machine Code (JIT)":   10}

STRUCTURES = {
    "Static Array":         1,
    "Dynamic Array":        2,
    "Stacks/Queues":        3,
    "Singly Linked List":   4,
    "Binary Decision Tree": 5,
    "Hash Map":             6}

ENGINES = {
    "Switch-Case":    {
        "source":      "../switch.c",
        "binary":      BUILD / "switch",
        "techniques": ["Switch-Case"],    
        "structures": [],
        "profile": BUILD / "profile_switch"},
    "Tail-Calling":   {
        "source":      "../tail-call.c",
        "binary":      BUILD / "tail-call",
        "techniques": ["Tail-Calling"],   
        "structures": ["Static Array"],
        "profile": BUILD / "profile_tail-call"},
    "Computed-Goto":   {
        "source":      "../computed-goto.c",
        "binary":      BUILD / "computed-goto", 
        "techniques": ["Computed-Goto"],   
        "structures": ["Static Array"],
        "profile": BUILD / "profile_computed-goto"},
    "Block Decoding": {
        "source":      "../blocked.c",
        "binary":      BUILD / "blocked",
        "techniques": ["Block Decoding", 
                       "Computed-Goto"], 
        "structures": ["Static Array", 
                       "Dynamic Array", 
                       "Binary Decision Tree", 
                       "Hash Map"],
        "profile": BUILD / "profile_blocked"}}

# State holders

def gather_performance():
    ASLR_ORIG = ""
    GOVN_ORIG = ""
    SIGNAL_HANDLER = None

    def cleanup(signum=None, frame=None):
        global ASLR_ORIG, GOVN_ORIG, SIGNAL_HANDLER
        RESULTS.mkdir(exist_ok=True)
        print("Unstaging environment: Efficiency mode, ASLR on...")
        with open("/proc/sys/kernel/randomize_va_space", "w") as f:
            f.write(ASLR_ORIG)
        with open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "w") as f:
            f.write(GOVN_ORIG)
        signal.signal(signal.SIGINT, SIGNAL_HANDLER)

    def startup():
        global ASLR_ORIG, GOVN_ORIG, SIGNAL_HANDLER
        SIGNAL_HANDLER = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, cleanup)
        print("Staging environment: Performance mode, ASLR off...")
        with open("/proc/sys/kernel/randomize_va_space", "r") as f:
            ASLR_ORIG = f.read().strip()
        with open("/proc/sys/kernel/randomize_va_space", "w") as f:
            f.write("0")
        with open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "r") as f:
            GOVN_ORIG = f.read().strip()
        with open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "w") as f:
            f.write("performance")


    def profile(interpreter, benchmark) -> dict:
        cmd = ["perf", "stat", "-x", ",", 
                               "-r", RUNS, 
                               "taskset", "-c", CORE, interpreter, benchmark]

        perf_return = subprocess.run(cmd, check=True, 
                                     stderr=subprocess.PIPE, 
                                     stdout=subprocess.DEVNULL)

        perf_stat = perf_return.stderr\
                               .decode('utf-8')\
                               .split('\n')

        measurements = {}
        for entry in perf_stat:
            if (len(entry := entry.split(',')) > 3):
                name = entry[2]
                if "task-clock" in name:
                    measurements[entry[2]] = entry[0]
                if "core" not in name:
                    continue
                if "cycles" in name:
                    measurements[entry[2]] = entry[0]
                if "instructions" in name:
                    measurements[entry[2]] = entry[0]
                if "topdown" in name:
                    """ handle tmam """
                    measurements[entry[7]] = entry[6]

        return measurements

    startup()

    for engine, econfig in ENGINES.items():
        print(f"Processing Engine: {engine}")
        interpreter = str(econfig['binary'])

        for benchmark, binary in BENCHMARKS.items():
            try:                                       
                ENGINES[engine][benchmark] =\
                    profile(interpreter, str(binary))
            except subprocess.CalledProcessError as e: 
                cleanup(); sys.exit(0)

    cleanup()


# gather lines of code count
def gather_complexity():
    FORMULA = lambda engine: (sum(TECHNIQUES[t] * W_DISPATCH for t in engine["techniques"]) + \
                              sum(STRUCTURES[s] * W_DATA     for s in engine["structures"]) + \
                              engine["nloc"] * W_NLOC)

    for engine, config in ENGINES.items():
        stats                        = lizard.analyze_file(config["source"])
        config["nloc"]               = stats.nloc
        config["cc"]                 = sum(func.cyclomatic_complexity for func in stats.function_list)
        config["fan-out"]            = len(stats.function_list)
        ENGINES[engine]['heuristic'] =  FORMULA(config)

def gather_efficiency():
    PROGRAMS = ['daxpy']

    FORMULA = lambda instruction, interpreter: \
            100 * instruction / (interpreter-instruction)


    def profile(binary: str, benchmark: str):
        cmd = [
            os.path.join(os.environ['PIN_ROOT'], 'pin'), 
            '-t', '../pintool/obj-intel64/InterpreterEfficiency.so',
            '--', binary, benchmark
        ]
        
        try:
            output = subprocess.run(cmd, check=True, capture_output=True, text=True)
        except subprocess.CalledProcessError as e:
            print(f"An error occurred while running the PIN tool: {e}")
            return 1

        match = re.search(r'Data:([^\r\n]+)', output.stdout)
        if match:
            return match.group(1).strip().split(',')

        return 0


    for engine, econfig in ENGINES.items():
        print(f"Processing Engine: {engine}")
        interpreter = str(econfig['profile'])
        for benchmark, binary in BENCHMARKS.items():
            results = profile(interpreter, str(binary))

            ENGINES[engine][benchmark]['Processing Instructions'] = results[0]
            ENGINES[engine][benchmark]['Dispatching Instructions'] = results[1]
            ENGINES[engine][benchmark]['Interpreter Efficiency'] = results[2]

            print(ENGINES[engine])

# Your data structure is already perfectly set up for this loop:
def export_to_csv(filename="benchmarks.csv"):
    # Headers include metadata + benchmark context + metrics
    headers = ['Engine', 'Benchmark', 'Heuristic', 'NLOC', 'CC', 'Fan-Out', 
               "Time (msec)", "Instrs", "Cycles", "Retiring", "Frontend", 
               "Backend", "Bad Spec", "Branch Mispredict", "Memory", 
               "Processing Instructions", "Dispatching Instructions", 
               "Interpreter Efficiency"]

    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)

        for engine_name, engine_data in ENGINES.items():
            # Extract shared metadata
            heuristic      = engine_data.get('heuristic') 
            nloc           = engine_data.get('nloc')
            cc             = engine_data.get('cc')
            fan_out        = engine_data.get('fan-out')
            efficiency     = engine_data.get('efficiency')

            # Iterate over the benchmark keys (daxpy, linked_list, quicksort)
            for key, val in engine_data.items():
                # Only process items that are dictionaries (the benchmark stats)
                if isinstance(val, dict):
                    writer.writerow([
                        engine_name,
                        key, # This is the "Benchmark" column
                        heuristic,
                        nloc,
                        cc,
                        fan_out,
                        val.get("task-clock"),
                        val.get("cpu_core/instructions/"),
                        val.get("cpu_core/cycles/"),
                        val.get("Retiring"),
                        val.get("Frontend Bound"),
                        val.get("Backend Bound"),
                        val.get("Bad Speculation"),
                        val.get("Branch Mispredict"),
                        val.get("Memory Bound"),
                        val.get("Processing Instructions"),
                        val.get("Dispatching Instructions"),
                        val.get("Interpreter Efficiency")
                    ])

if __name__ == "__main__":
    gather_performance()
    gather_efficiency()
    gather_complexity()
    export_to_csv()
