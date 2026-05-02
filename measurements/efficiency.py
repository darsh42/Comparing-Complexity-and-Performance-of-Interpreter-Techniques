import os
import re
import csv

from pathlib import Path
from parallel import parallel_execute

PINROOT = os.environ.get('PIN_ROOT')
if not PINROOT:
    raise EnvironmentError('PIN_ROOT not defined')

BUILD   = Path("./../build")
RESULTS = Path("./../build/measurements")
ECORES = [12, 13]
BENCHMARKS = {"DAXPY":  BUILD / "samples/daxpy",
              "LL":     BUILD / "samples/linked_list",
              "QS":     BUILD / "samples/quicksort",
              "CRC":    BUILD / "samples/crc",
              "BS":     BUILD / "samples/binary_search",
              "NPB IS": BUILD / "samples/bin/is.W.x"}
ENGINES    = {"SC":     BUILD / "profile_switch",
              "TC":     BUILD / "profile_tail-call",
              "CG":     BUILD / "profile_computed-goto",
              "BD":     BUILD / "profile_blocked"}

COMMAND = lambda core, interpreter, workload: ['taskset', '-c', str(core), 
                                               str(Path(PINROOT) / 'pin'), 
                                                   '-t', '../pintool/obj-intel64/InterpreterEfficiency.so', 
                                                   '--', str(interpreter), str(workload)]

RESULTS.mkdir(exist_ok=True)

def process_efficiency(data, filename=str(RESULTS / 'efficiency.csv')):
    processed = []
    for entry in data:
        engine, benchmark, results = entry.values()
        
        results = results.stdout

        if (raw := re.search(r'Data:([^\r\n]+)', results)):
            processed.append([engine, benchmark] + raw.group(1).strip().split(','))

    headers = ['Engine', 'Benchmark', 'Processing Instructions', 
               'Dispatching Instructions', 'Interpreter Efficiency']
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        writer.writerows(processed)

if __name__ == '__main__':
    process_efficiency(parallel_execute(COMMAND, ECORES, ENGINES, BENCHMARKS))
