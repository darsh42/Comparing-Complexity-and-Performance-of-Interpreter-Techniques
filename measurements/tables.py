import pandas as pd
import re

LATEX_SPECIALS = {
    '&': r'\&',
    '%': r'\%',
    '$': r'\$',
    '#': r'\#',
    '_': r'\_',
    '{': r'\{',
    '}': r'\}',
    '~': r'\textasciitilde{}',
    '^': r'\^{}',
    '\\': r'\textbackslash{}',
}

def latex_escape(val):
    if isinstance(val, str):
        for k, v in LATEX_SPECIALS.items():
            val = val.replace(k, v)
    return val

# =========================================================
# 1. LOAD + CLEAN
# =========================================================
df = pd.read_csv("benchmarks.csv", index_col=False)

df.columns = df.columns.str.strip()
df['Engine'] = df['Engine'].astype(str).str.strip()
df['Benchmark'] = df['Benchmark'].astype(str).str.strip()

# calculate IPC
df['IPC'] = df['Instrs'] / df['Cycles']

# derived metric
df['Cycles (10^9)'] = df['Cycles'] / 1e9
df = df.drop(columns=['Cycles'])

# =========================================================
# 2. ENGINE ORDER (IMPORTANT FOR READABILITY)
# =========================================================
engine_order = ['Switch-Case', 'Tail-Calling', 'Computed-Goto', 'Block Decoding']

df['Engine'] = pd.Categorical(
    df['Engine'],
    categories=engine_order,
    ordered=True
)

# =========================================================
# 3. IEEE-SAFE FORMATTING (NO siunitx, NO scientific notation)
# =========================================================
def fmt_float(x, d=2):
    return f"{x:.{d}f}"

def fmt_int(x):
    return f"{int(x)}"

df['Time (msec)']   = df['Time (msec)'].map(fmt_float)
df['Cycles (10^9)'] = df['Cycles (10^9)'].map(fmt_float)
df['IPC']           = df['IPC'].map(fmt_float)

# microarchitectural percentages (if present)
micro_cols = [
    'Frontend', 'Backend',
    'Retiring', 'Bad Spec',
    'Branch Mispredict', 'Memory'
]

for c in micro_cols:
    if c in df.columns:
        df[c] = df[c].map(fmt_float)

# =========================================================
# 4. TABLE 1 — PERFORMANCE
# =========================================================
rq1 = (
    df[['Benchmark', 'Engine', 'Time (msec)', 'IPC', 'Cycles (10^9)']]
    .sort_values(['Benchmark', 'Engine'])
    .set_index(['Benchmark', 'Engine'])
)

# =========================================================
# 5. TABLE 2 — COMPLEXITY
# =========================================================
rq2 = (
    df[['Engine',
        'Heuristic',
        'CC',
        'Fan-Out',
        'NLOC']]
    .drop_duplicates()
    .sort_values('Heuristic')
    .set_index('Engine')
)

# =========================================================
# 6. TABLE 3 — MICROARCH
# =========================================================
rq3 = (
    df[['Benchmark', 'Engine'] + micro_cols]
    .sort_values(['Benchmark', 'Engine'])
    .set_index(['Benchmark', 'Engine'])
)

# =========================================================
# 7. IEEE LATEX EXPORT FUNCTION (SAFE)
# =========================================================
def latex_ieee(table, caption, label, colspec):
    latex_body = table.to_latex(
        escape=True,
        header=True,
        index=True,
        multirow=True
    )

    # remove pandas tabular wrapper
    latex_body = latex_body.split('\n', 1)[1]
    latex_body = latex_body.rsplit('\\end{tabular}', 1)[0]

    return f"""
\\begin{{table*}}[t]
\\centering
\\caption{{{caption}}}
\\label{{{label}}}
\\begin{{tabularx}}{{\\textwidth}}{{{colspec}}}
{latex_body}
\\end{{tabularx}}
\\end{{table*}}
"""

# =========================================================
# 8. COLUMN SPECIFICATIONS (IEEE SAFE)
# =========================================================

col1 = "llXXX"
col2 = "lXXXX"
col3 = "llXXXXXXX"

# =========================================================
# 9. OUTPUT TABLES
# =========================================================

with open("./tables/Interpreter Performance Across Benchmarks.tex", "w") as f:
    f.write(latex_ieee(rq1, "Interpreter Performance Across Benchmarks", "tab:performance", col1))

with open("./tables/Interpreter Implementation Complexity.tex", "w") as f:
    f.write(latex_ieee(rq2, "Interpreter Implementation Complexity", "tab:complexity", col2))

with open("./tables/Microarchitectural Breakdown of Interpreter Designs.tex", "w") as f:
    f.write(latex_ieee(rq3, "Microarchitectural Breakdown of Interpreter Designs", "tab:microarch", col3))
