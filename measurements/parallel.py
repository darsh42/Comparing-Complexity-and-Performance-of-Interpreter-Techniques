import os
import itertools
import subprocess

from concurrent.futures import ProcessPoolExecutor

def profile(args):
    """ 
        executes the spawned command
        args:
            command   -> [str]
            engine    ->  str
            benchmark -> str
    """
    command, engine, benchmark = args

    print(' '.join(command))

    try:
        results = subprocess.run(command, capture_output=True,
                                          check=True,
                                          text=True)
    except subprocess.CalledProcessError as e:
        print(e.stdout); exit(-1)

    return {'engine': engine, 'benchmark': benchmark, 'results': results}

def parallel_execute(command, cores, engines, benchmarks):
    """
        spawns a worker for combinations of interpreters and benchmarks executing a command
        args:
            command    -> lambda core, interpreter, workload: ...
            engines    -> list of engines and the associated binaries
            benchmarks -> list of benchmarks and the associated binaries
    """
    tasks  = []
    cycler = itertools.cycle(cores)
    for benchmark, workload in benchmarks.items():
        for engine, interpreter in engines.items():
            tasks.append([command(next(cycler), interpreter, workload),
                          engine, benchmark])
    results = []
    with ProcessPoolExecutor(max_workers=len(cores)) as executor:
        results = list(executor.map(profile, tasks))
    return results
