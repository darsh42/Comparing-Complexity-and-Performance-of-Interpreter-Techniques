import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

plt.rcParams.update({
    "font.size": 8,
    "axes.titlesize": 9,
    "axes.labelsize": 8,
    "legend.fontsize": 7,
    "hatch.linewidth": 0.2
})

fig_width  = 3.5
fig_height = 3.5

(fig_width, fig_height)

# =========================================================
# 1. LOAD + CLEAN
# =========================================================
df = pd.read_csv("benchmarks.csv", index_col=False)

df.columns = df.columns.str.strip()
df['Engine'] = df['Engine'].astype(str).str.strip()
df['Benchmark'] = df['Benchmark'].astype(str).str.strip()

benchmark_order = [
    'DAXPY',
    'QS',
    'LL',
    'BS',
    'CRC',
    'IS'
]
engine_order = [
    'Switch-Case', 
    'Tail-Calling', 
    'Computed-Goto', 
    'Block Decoding'
]

df['Benchmark'] = pd.Categorical(df['Benchmark'], categories=benchmark_order, ordered=True)
df['Engine']    = pd.Categorical(df['Engine'],    categories=engine_order,    ordered=True)

df = df.sort_values(['Benchmark', 'Engine'])

df['Cycles (10^9)'] = df['Cycles'] / 1e9

os.makedirs("figures", exist_ok=True)

# =========================================================
# 2. HELPERS
# =========================================================
engine_order = ['Switch-Case', 'Tail-Calling', 'Computed-Goto', 'Block Decoding']

greymap = {
    'Switch-Case': '0.4',
    'Tail-Calling': '0.55',
    'Computed-Goto': '0.70',
    'Block Decoding': '0.85'
}

tmamhatch = {
    'Bad Spec': '/////////////',
    'Retiring': '',
    'Frontend': '\\\\\\\\\\\\\\\\\\\\\\\\\\',
    'Backend': '---------------',
}

def savefig(name):
    plt.tight_layout()
    plt.savefig(f"figures/{name}.pdf", bbox_inches='tight')
    plt.close()

# =========================================================
# RQ1 — RUNTIME PERFORMANCE
# =========================================================

# -------------------------
# FIG 1: runtime bar chart
# -------------------------
def plot_runtime_bar():
    pivot = df.pivot(index='Benchmark', columns='Engine', values='Time (msec)')
    pivot = pivot[engine_order]

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))  # IEEE single column

    n_benchmarks = len(pivot.index)
    n_engines    = len(engine_order)
    bar_width = 0.18

    x = np.arange(n_benchmarks)

    for i, engine in enumerate(engine_order):
        bars = ax.bar(
            x + i * bar_width,
            pivot[engine]/pivot['Switch-Case'],
            width=bar_width,
            edgecolor='black',
            color=greymap[engine],
            label=engine
        )

    ax.set_xticks(x + bar_width * (n_engines - 1) / 2)
    ax.set_xticklabels(pivot.index.str.replace("_", " ", regex=False),rotation=30, ha='right')

    ax.set_ylabel("Relative Speedup vs. Switch-Case")

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=2,              # or 4 if you want one row
        frameon=False
    )

    savefig("barchart_runtime")

# =========================================================
# RQ2 — COMPLEXITY TRADEOFFS
# =========================================================

# =========================================================
# RQ3 — MICROARCHITECTURE
# =========================================================

micro_cols = [
    'Frontend','Backend','Retiring',
    'Bad Spec','Branch Mispredict','Memory'
]

import matplotlib.colors as mcolors
from matplotlib.patches import Patch

def plot_microarch_bar():
    # 1. Define distinct base colors for each TMAM metric (Hues)
    # Using hex codes converted to RGB
    metric_base_colors = {
        'Retiring': '#E41A1C', # Red
        'Frontend': '#377EB8', # Blue
        'Backend':  '#4DAF4A', # Green
        'Bad Spec': '#984EA3'  # Purple
    }

    tmam_cols = ['Retiring', 'Frontend', 'Backend', 'Bad Spec']
    
    benchmarks = df['Benchmark'].cat.categories
    engines    = df['Engine'].cat.categories

    x = np.arange(len(benchmarks))
    width = 0.18

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    for i, engine in enumerate(engines):
        subset = df[df['Engine'] == engine]
        
        # Get the intensity factor for this engine (cast to float)
        intensity = float(greymap[engine])
        
        bottom = np.zeros(len(benchmarks))

        for col in tmam_cols:
            values = subset[col].values
            
            # Calculate the shaded color:
            # 1. Convert base color to (R, G, B) tuple
            # 2. Multiply each channel by the intensity
            base_rgb = mcolors.to_rgb(metric_base_colors[col])
            shaded_color = [c * intensity for c in base_rgb]

            ax.bar(
                x + i * width,
                values,
                width,
                bottom=bottom,
                color=shaded_color,  # The hue-intensity mix
                edgecolor='black',
                linewidth=0.3,
                label=col if i == 0 else "" # Only add label once
            )

            bottom += values

    # X labels
    ax.set_xticks(x + width * (len(engines) - 1) / 2)
    ax.set_xticklabels(
        [b.replace("_", " ") for b in benchmarks],
        rotation=30,
        ha='right',
        fontsize=8
    )

    ax.set_ylabel("Pipeline Slots (%)")

    # 1. Legend for TMAM Metrics (The Hues)
    metric_handles = [ Patch(facecolor=metric_base_colors[col], edgecolor='black', label=col) for col in tmam_cols ]
    engine_handles = [ Patch(facecolor=str(val), edgecolor='black', label=engine) for engine, val in greymap.items() ]
    legend = ax.legend(
        handles=metric_handles+engine_handles,
        loc='upper center',
        bbox_to_anchor=(0.5, -0.15),
        ncol=4,
        frameon=False,
        fontsize=6
    )
    savefig("barchart_tmam")

def plot_complexity_runtime():

    # -----------------------------
    # Normalisation (Switch-Case baseline per benchmark)
    # -----------------------------
    baseline = (
        df[df['Engine'] == 'Switch-Case']
        .set_index('Benchmark')['Time (msec)']
    )

    df['Time_norm'] = df.apply(
        lambda r: r['Time (msec)'] / baseline[r['Benchmark']],
        axis=1
    )

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    colors = plt.cm.tab10.colors

    markers = {
        'Switch-Case': 'o',
        'Tail-Calling': 's',
        'Computed-Goto': '^',
        'Block Decoding': 'D'
    }

    benchmarks = df['Benchmark'].unique()

    # -----------------------------
    # ONE LINE PER BENCHMARK (FIX)
    # -----------------------------
    for i, bench in enumerate(benchmarks):
        subset = df[df['Benchmark'] == bench].sort_values('Engine')

        x = subset['Heuristic']
        y = subset['Time_norm']

        # main line (continuous)
        ax.plot(
            x, y,
            color=colors[i % len(colors)],
            linewidth=1.2,
            label=bench
        )

        # overlay markers per engine
        for _, row in subset.iterrows():
            ax.scatter(
                row['Heuristic'],
                row['Time_norm'],
                marker=markers[row['Engine']],
                color=colors[i % len(colors)],
                s=18
            )

    ax.set_xlabel("Interpreter Complexity")
    ax.set_ylabel("Runtime (Normalised to Switch-Case)")

    ax.grid(True, linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=3,
        frameon=False,
        fontsize=7
    )

    savefig('linegraph_runtime_complexity')

def plot_complexity_barchart():
    """
    IEEE-style complexity comparison bar chart.
    """
    # -----------------------------
    # Derived metric (safe, local)
    # -----------------------------
    df['NLOC (×10)'] = df['NLOC'] / 10

    # -----------------------------
    # Select + align data
    # -----------------------------
    cols = [
        'Engine',
        'Heuristic',
        'CC',
        'Fan-Out',
        'NLOC (×10)'
    ]

    data = (
        df[cols]
        .drop_duplicates()
        .set_index('Engine')
        .loc[engine_order]
    )

    # -----------------------------
    # Visual encoding (IEEE-safe)
    # -----------------------------
    greys = ['0.2', '0.4', '0.6', '0.8']

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    metrics = [
        'Heuristic',
        'CC',
        'Fan-Out',
        'NLOC (×10)'
    ]

    x = range(len(metrics))
    width = 0.18

    # -----------------------------
    # Plot grouped bars
    # -----------------------------
    for i, engine in enumerate(engine_order):
        ax.bar(
            [pos + i * width for pos in x],
            data.loc[engine, metrics],
            width=width,
            label=engine,
            color=greys[i],
            edgecolor='black',
            linewidth=0.6
        )

    # -----------------------------
    # Formatting
    # -----------------------------
    ax.set_xticks([r + 1.5 * width for r in x])
    ax.set_xticklabels(metrics, rotation=25, ha='right')

    ax.set_ylabel("Value")

    ax.grid(axis='y', linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=2,
        frameon=False,
        fontsize=7
    )

    savefig("barchart_complexity")

def plot_efficiency():
    # -----------------------------
    # Keep only rows with efficiency
    # -----------------------------

    # -----------------------------
    # Enforce ordering
    # -----------------------------
    engine_order = ['Switch-Case', 'Tail-Calling', 'Computed-Goto', 'Block Decoding']
    df['Engine'] = pd.Categorical(df['Engine'], categories=engine_order, ordered=True)

    benchmark_order = df['Benchmark'].unique()

    # -----------------------------
    # Pivot for plotting
    # -----------------------------
    pivot = (
        df
        .pivot(index='Benchmark', columns='Engine', values='Interpreter Efficiency')
        .loc[benchmark_order, engine_order]
    )

    # -----------------------------
    # Plot (IEEE-friendly)
    # -----------------------------
    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    x = range(len(pivot.index))
    bar_width = 0.18

    greys = ['0.2', '0.4', '0.6', '0.8']

    for i, engine in enumerate(engine_order):
        ax.bar(
            [pos + i * bar_width for pos in x],
            pivot[engine],
            width=bar_width,
            label=engine,
            color=greys[i],
            edgecolor='black',
            linewidth=0.6
        )

    # -----------------------------
    # Formatting
    # -----------------------------
    ax.set_xticks([pos + 1.5 * bar_width for pos in x])
    ax.set_xticklabels(pivot.index.str.replace("_", " "), rotation=25, ha='right')

    ax.set_ylabel("Interpreter Efficiency")

    ax.grid(axis='y', linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=2,
        frameon=False,
        fontsize=7
    )
    
    savefig("barchart_efficiency")

def plot_instruction_counts():
    # 1. Filter for only the Switch-Case engine
    subset = df[df['Engine'] == 'Switch-Case'].copy()

    # 2. Calculate sum and log10 scale
    # Ensure these columns exist in your CSV
    cols_to_sum = ['Dispatching Instructions', 'Processing Instructions']
    subset['Instruction Sum'] = np.log10(subset[cols_to_sum].sum(axis=1) + 1)

    # 3. Plot
    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    x = np.arange(len(subset['Benchmark']))

    ax.bar(
        x,
        subset['Instruction Sum'],
        width=0.35,
        color='0.4', # Match the grey used for Switch-Case in your previous plots
        edgecolor='black',
        linewidth=0.6
    )

    # 4. Formatting
    ax.set_xticks(x)
    ax.set_xticklabels(subset['Benchmark'].astype(str).str.replace("_", " "), rotation=30, ha='right')

    ax.set_ylabel("Instructions (Log10)")
    ax.grid(axis='y', linewidth=0.3, alpha=0.5)

    savefig("barchart_instruction_counts_switch_case")

def plot_branch_misprediction():
    """
    Bar chart for Branch Misprediction across engines and benchmarks.
    """
    # Pivot for plotting
    pivot = df.pivot(index='Benchmark', columns='Engine', values='Branch Mispredict')
    pivot = pivot[engine_order]

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    x = np.arange(len(pivot.index))
    bar_width = 0.18

    for i, engine in enumerate(engine_order):
        ax.bar(
            x + i * bar_width,
            pivot[engine],
            width=bar_width,
            color=greymap[engine],
            edgecolor='black',
            label=engine,
            linewidth=0.6
        )

    # Formatting
    ax.set_xticks(x + bar_width * (len(engine_order) - 1) / 2)
    ax.set_xticklabels(pivot.index.str.replace("_", " "), rotation=30, ha='right')

    ax.set_ylabel("Branch Mispredict (%)")
    ax.grid(axis='y', linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=2,
        frameon=False,
        fontsize=7
    )
    
    savefig("barchart_branch_misprediction")

def plot_memory():
    """
    Bar chart for Memory metrics across engines and benchmarks.
    """
    # Pivot for plotting
    pivot = df.pivot(index='Benchmark', columns='Engine', values='Memory')
    pivot = pivot[engine_order]

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))

    x = np.arange(len(pivot.index))
    bar_width = 0.18

    for i, engine in enumerate(engine_order):
        ax.bar(
            x + i * bar_width,
            pivot[engine],
            width=bar_width,
            color=greymap[engine],
            edgecolor='black',
            label=engine,
            linewidth=0.6
        )

    # Formatting
    ax.set_xticks(x + bar_width * (len(engine_order) - 1) / 2)
    ax.set_xticklabels(pivot.index.str.replace("_", " "), rotation=30, ha='right')

    ax.set_ylabel("Memory (%)")
    ax.grid(axis='y', linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, -0.25),
        ncol=2,
        frameon=False,
        fontsize=7
    )
    
    savefig("barchart_memory_bound")

# =========================================================
# 3. RUN ALL FIGURES
# =========================================================
if __name__ == "__main__":
    plot_runtime_bar()
    plot_microarch_bar()
    plot_complexity_runtime()
    plot_complexity_barchart()
    plot_efficiency()
    plot_instruction_counts()
    plot_branch_misprediction()
    plot_memory()
    print("All figures generated in ./figures/")
