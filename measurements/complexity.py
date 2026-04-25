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
        "techniques": ["Switch-Case"],    
        "structures": []},
    "Tail-Calling":   {
        "source":      "../tail-call.c",
        "techniques": ["Tail-Calling"],   
        "structures": ["Static Array"]},
    "Computed-Goto":   {
        "source":      "../computed-goto.c",
        "techniques": ["Computed-Goto"],   
        "structures": ["Static Array"]},
    "Block Decoding": {
        "source":      "../blocked.c",
        "techniques": ["Block Decoding",
                       "Computed-Goto",
                       "Tail-Calling"], 
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
                         
                         

    headers = ['Engine', 'Heuristic', 'NLOC', 'CC', 'Fan-Out']
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)

        writer.writerow(headers)
        writer.writerows(processed)

if __name__ == '__main__':
    process_complexity()
