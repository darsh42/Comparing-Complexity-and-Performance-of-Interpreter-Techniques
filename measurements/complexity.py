import csv
import lizard

from pathlib import Path

RESULTS = Path("./../build/measurements")

N_INSTRUCTIONS = 64

W_DISPATCH = 0.70
W_DATA     = 0.29
W_NLOC     = 0.01

TECHNIQUES = {
    "Switch-Case":          1,
    "Token-Threading":      2,
    "Tail-Calling":         3,
    "Computed-Goto":        4,
    "Super Instructions":   5,
    "Context Threading":    6,
    "Block Decoding":       7,
    "Machine Code (JIT)":   8}

STRUCTURES = {
    "Static Array":         1,
    "Dynamic Array":        2,
    "Stacks/Queues":        3,
    "Singly Linked List":   4,
    "Binary Decision Tree": 5,
    "Hash Map":             6}

ENGINES = {
    "SC":    {
        "source":      "../switch.c",
        "techniques": ["Switch-Case"],    
        "structures": []},
    "TC":   {
        "source":      "../tail-call.c",
        "techniques": ["Tail-Calling"],   
        "structures": ["Static Array"]},
    "CG":   {
        "source":      "../computed-goto.c",
        "techniques": ["Computed-Goto"],   
        "structures": ["Static Array"]},
    "BD": {
        "source":      "../blocked.c",
        "techniques": ["Block Decoding",
                       "Computed-Goto"], 
        "structures": ["Static Array",
                       "Dynamic Array", 
                       "Binary Decision Tree", 
                       "Hash Map"]}}

FORMULA = lambda techniques, structures, nloc: (sum(TECHNIQUES[t] * W_DISPATCH for t in techniques)+\
                                                sum(STRUCTURES[s] * W_DATA     for s in structures)+\
                                                       nloc * W_NLOC)

def process_complexity(filename=str(RESULTS / 'complexity.csv')):
    # gather lines of code count
    processed = []
    for engine, config in ENGINES.items():
        stats = lizard.analyze_file(config["source"])

        nloc    = stats.nloc
        cc      = sum(func.cyclomatic_complexity for func in stats.function_list)
        fan_out = len(stats.function_list)


        processed.append([engine, nloc, cc, fan_out,
                          FORMULA(config['techniques'],
                                  config['structures'],
                                  nloc)])

    headers = ['Engine', 'NLOC', 'CC', 'Fan-Out', 'Heuristic']
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)

        writer.writerow(headers)
        writer.writerows(processed)

if __name__ == '__main__':
    process_complexity()
