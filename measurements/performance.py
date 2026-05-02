import os
import re
import csv
import signal
import itertools

from pathlib  import Path
from parallel import parallel_execute

RUNS    = "100"
BUILD   = Path("./../build")
RESULTS = Path("./../build/measurements")
PCORES = [0, 2]
BENCHMARKS = {"DAXPY":  BUILD / "samples/daxpy",
              "LL":     BUILD / "samples/linked_list",
              "QS":     BUILD / "samples/quicksort",
              "CRC":    BUILD / "samples/crc",
              "BS":     BUILD / "samples/binary_search",
              "NPB IS": BUILD / "samples/bin/is.W.x"}
ENGINES    = {"SC":     BUILD / "switch",
              "TC":     BUILD / "tail-call",
              "CG":     BUILD / "computed-goto",
              "BD":     BUILD / "blocked"}
CMD  = lambda core, interpreter, workload: ["perf", "stat", "-x", ",", 
                                                            "-r", RUNS,
                                                            "taskset", "-c", str(core), 
                                                                             str(interpreter), 
                                                                             str(workload)]

RESULTS.mkdir(exist_ok=True)

# Environment Management
ASLR_ORIG = ""
GOVN_ORIG = ""
SIGNAL_HANDLER = None

def cleanup(signum=None, frame=None):
    global ASLR_ORIG, GOVN_ORIG, SIGNAL_HANDLER
    print("Unstaging environment: Efficiency mode, ASLR on...")
    if ASLR_ORIG:
        with open("/proc/sys/kernel/randomize_va_space", "w") as f:
            f.write(ASLR_ORIG)
    if GOVN_ORIG:
        with open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "w") as f:
            f.write(GOVN_ORIG)
    if SIGNAL_HANDLER:
        signal.signal(signal.SIGINT, SIGNAL_HANDLER)
    exit(0)

def setup():
    global ASLR_ORIG, GOVN_ORIG, SIGNAL_HANDLER
    if os.geteuid() != 0:
        print("Error: Must run as root to modify CPU governor and ASLR.")
        exit(-1)
        
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

def process_performance(data, filename=str(RESULTS / 'performance.csv')):
    processed = []
    for entry in data:
        engine, benchmark, results = entry.values()

        results = results.stderr.split('\n')
        
        measurements = {}
        for row in results:
            if (len(row := row.split(',')) < 3):
                continue

            name = row[2]
            if "task-clock"   in name: measurements[row[2]] = row[0]

            if "core" not in name:
                continue

            if "cycles"       in name: measurements[row[2]] = row[0]
            if "instructions" in name: measurements[row[2]] = row[0]
            if "topdown"      in name: measurements[row[7]] = row[6]

        processed.append([engine, benchmark, measurements["task-clock"            ],
                                             measurements["cpu_core/instructions/"],
                                             measurements["cpu_core/cycles/"      ],
                                             measurements["Retiring"              ],
                                             measurements["Frontend Bound"        ],
                                             measurements["Backend Bound"         ],
                                             measurements["Bad Speculation"       ],
                                             measurements["Branch Mispredict"     ],
                                             measurements["Memory Bound"          ]])

    headers = ["Engine", "Benchmark", "Time (msec)", "Instructions", "Cycles", "Retiring", 
               "Frontend", "Backend", "Bad Spec", "Branch Mispredict", "Memory"]

    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        writer.writerows(processed)

if __name__ == '__main__':
    setup(); process_performance(parallel_execute(CMD, PCORES, ENGINES, BENCHMARKS)); cleanup()
