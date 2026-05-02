import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

fig_width  = 3.2
fig_height = 4

engine_colors   = {
    'SC': '#3c1518',
    'TC': '#69140e',
    'CG': '#a44200',
    'BD': '#d58936',
}

engine_order    = ['SC', 'TC', 'CG', 'BD']
benchmark_order = ['DAXPY', 'QS', 'LL', 'BS', 'CRC', 'NPB IS']

# load data frames
df_complexity  = pd.read_csv('../build/measurements/complexity.csv',  index_col=False)
df_efficiency  = pd.read_csv('../build/measurements/efficiency.csv',  index_col=False)
df_performance = pd.read_csv('../build/measurements/performance.csv', index_col=False)

# clean data frames
def clean(df):
    df.columns = df.columns.str.strip()

    sort_values = []
    if 'Benchmark' in df:
        sort_values.append('Benchmark')
        df['Benchmark'] = pd.Categorical(df['Benchmark'].astype(str).str.strip(),
                                         categories=benchmark_order,
                                         ordered=True)
    if 'Engine' in df:
        sort_values.append('Engine')
        df['Engine'] = pd.Categorical(df['Engine'].astype(str).str.strip(),
                                      categories=engine_order,
                                      ordered=True)

    return df.sort_values(sort_values)

df_complexity  = clean(df_complexity)
df_efficiency  = clean(df_efficiency)
df_performance = clean(df_performance)

print(df_complexity)
print(df_efficiency)
print(df_performance)

# transform data frames

# helpers
def save_figure(name):
    plt.tight_layout()
    plt.savefig(f"figures/{name}.pdf", bbox_inches='tight')
    plt.close()

"""
    H1: 
        runtime 
        instruction counts
"""
def plot_bargraph_runtime(df):
    barwidth = 0.18

    n_engines    = len(engine_order)
    n_benchmarks = len(benchmark_order)

    x = np.arange(n_benchmarks)

    fig, ax = plt.subplots(figsize=(fig_width, 
                                    fig_height))

    df    = df.copy()
    pivot = df.pivot(index='Benchmark', 
                     columns='Engine', 
                     values='Time (msec)')[engine_order]

    for i, engine in enumerate(engine_order):
        bars = ax.bar(x+i*barwidth, pivot[engine]/pivot['SC'], label=engine,
                                                               width=barwidth,
                                                               color=engine_colors[engine])

    ax.set_xticks(x+barwidth * (n_engines-1)/2)
    ax.set_xticklabels(pivot.index, rotation=30, ha='right')

    ax.set_ylabel('Relative Speedup vs. Switch-Case')

    ax.legend(loc='upper center', 
              bbox_to_anchor=(0.5, -0.25), 
              ncol=2, frameon=False)
    
    save_figure("bargraph_runtime")

def plot_bargraph_instruction_counts(df):
    barwidth = 0.18

    n_engines    = len(engine_order)
    n_benchmarks = len(benchmark_order)

    x = np.arange(n_benchmarks)

    fig, ax = plt.subplots(figsize=(fig_width,
                                    fig_height))

    df    = df.copy()
    df['Total Instructions'] = df['Dispatching Instructions']+\
                               df[ 'Processing Instructions']

    pivot = df.pivot(index='Benchmark', 
                     columns='Engine', 
                     values='Total Instructions')[engine_order]

    pivot = np.log10(pivot+1)

    print(pivot)

    for i, engine in enumerate(engine_order):
        bars = ax.bar(x+i*barwidth, pivot[engine], label=engine,
                                                   width=barwidth,
                                                   color=engine_colors[engine])
    ax.set_xticks(x+barwidth * (n_engines-1)/2)
    ax.set_xticklabels(pivot.index, rotation=30, ha='right')

    ax.set_ylabel('Total Instruction Counts (Log10)')

    ax.legend(loc='upper center', 
              bbox_to_anchor=(0.5, -0.25), 
              ncol=2, frameon=False)
    
    save_figure("bargraph_instruction_counts")

"""
    H2:
        efficiency
"""

def plot_bargraph_efficiency(df):
    barwidth = 0.18

    n_engines    = len(engine_order)
    n_benchmarks = len(benchmark_order)

    x = np.arange(n_benchmarks)

    fig, ax = plt.subplots(figsize=(fig_width,
                                    fig_height))

    df    = df.copy()
    pivot = df.pivot(index='Benchmark', 
                     columns='Engine', 
                     values='Interpreter Efficiency')[engine_order]

    for i, engine in enumerate(engine_order):
        bars = ax.bar(x+i*barwidth, pivot[engine], label=engine,
                                                   width=barwidth,
                                                   color=engine_colors[engine])
    ax.set_xticks(x+barwidth * (n_engines-1)/2)
    ax.set_xticklabels(pivot.index, rotation=30, ha='right')

    ax.set_ylabel('Interpreter Efficiency')

    ax.legend(loc='upper center', 
              bbox_to_anchor=(0.5, -0.25), 
              ncol=2, frameon=False)
    
    save_figure("bargraph_interpreter_efficiency")

"""
    H3:
"""

import matplotlib.colors as mcolors
import colorsys  # Use the standard library for HLS conversion

def plot_stacked_barchart_tmam(df):
    tmam_categories = ['Retiring', 'Frontend', 'Backend', 'Bad Spec']

    # Lightness adjustment factor for each category
    metric_lightness = {
        'Retiring': 0.2,
        'Frontend': 0.1,
        'Backend': -0.1,
        'Bad Spec': -0.2
    }

    def get_color_shade(base_hex, metric):
        """Adjusts the lightness of the base hex color using colorsys."""
        # Convert hex to RGB (0-1 range)
        rgb = mcolors.to_rgb(base_hex)
        # Convert RGB to HLS
        h, l, s = colorsys.rgb_to_hls(*rgb)
        # Adjust Lightness and clip to [0, 1]
        new_l = np.clip(l + metric_lightness[metric], 0, 1)
        # Convert back to RGB
        return colorsys.hls_to_rgb(h, new_l, s)

    # Setup the figure
    fig, axes = plt.subplots(1, len(benchmark_order),
                             figsize=(fig_width * 3, fig_height),
                             sharey=True)

    for i, benchmark in enumerate(benchmark_order):
        ax = axes[i]

        # 1. Filter and sort
        subset = df[df['Benchmark'] == benchmark].set_index('Engine').reindex(engine_order)

        # 2. Track the bottom of the stack
        bottoms = np.zeros(len(engine_order))

        # 3. Plot each category
        for cat in tmam_categories:
            # Generate colors based on the engine's base color
            colors = [get_color_shade(engine_colors[e], cat) for e in subset.index]

            ax.bar(subset.index, subset[cat],
                   label=cat,
                   bottom=bottoms,
                   color=colors,
                   width=0.7)

            bottoms += subset[cat].values

        ax.set_title(benchmark)
        ax.set_ylim(0, 100)
        ax.set_ylabel('Percentage (%)' if i == 0 else '')
        ax.tick_params(axis='x', rotation=30)

    # Unified Legend
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='upper center',
               bbox_to_anchor=(0.5, 0.0), ncol=len(tmam_categories), frameon=False)

    plt.tight_layout()
    save_figure("tmam_stacked_bar_by_engine")
    


def plot_bargraph_bad_speculation(df):
    barwidth = 0.18

    n_engines    = len(engine_order)
    n_benchmarks = len(benchmark_order)

    x = np.arange(n_benchmarks)

    fig, ax = plt.subplots(figsize=(fig_width,
                                    fig_height))

    df    = df.copy()
    pivot = df.pivot(index='Benchmark', 
                     columns='Engine', 
                     values='Bad Spec')[engine_order]

    for i, engine in enumerate(engine_order):
        bars = ax.bar(x+i*barwidth, pivot[engine], label=engine,
                                                   width=barwidth,
                                                   color=engine_colors[engine])
    ax.set_xticks(x+barwidth * (n_engines-1)/2)
    ax.set_xticklabels(pivot.index, rotation=30, ha='right')

    ax.set_ylabel('Bad Speculation')

    ax.legend(loc='upper center', 
              bbox_to_anchor=(0.5, -0.25), 
              ncol=2, frameon=False)
    
    save_figure("bargraph_bad_speculation")


def plot_bargraph_memory_bound(df):
    barwidth = 0.18

    n_engines    = len(engine_order)
    n_benchmarks = len(benchmark_order)

    x = np.arange(n_benchmarks)

    fig, ax = plt.subplots(figsize=(fig_width,
                                    fig_height))

    df    = df.copy()
    pivot = df.pivot(index='Benchmark', 
                     columns='Engine', 
                     values='Memory')[engine_order]

    for i, engine in enumerate(engine_order):
        bars = ax.bar(x+i*barwidth, pivot[engine], label=engine,
                                                   width=barwidth,
                                                   color=engine_colors[engine])
    ax.set_xticks(x+barwidth * (n_engines-1)/2)
    ax.set_xticklabels(pivot.index, rotation=30, ha='right')

    ax.set_ylabel('Memory Bound')

    ax.legend(loc='upper center', 
              bbox_to_anchor=(0.5, -0.25), 
              ncol=2, frameon=False)
    
    save_figure("bargraph_memory_bound")

"""
    H4:
"""
def plot_bargraph_complexity(df):
    metric_order = ['Heuristic', 'CC', 'Fan-Out', 'NLOC (x10)']

    df = df.copy()

    df['NLOC (x10)'] = df['NLOC'] / 10
    df = df.drop(columns=['NLOC'])
    
    df = df.set_index('Engine').T
    
    df = df.reindex(metric_order)
    df = df[engine_order]

    colors = [engine_colors[e] for e in df.columns]
    
    ax = df.plot(kind='bar', figsize=(fig_width, fig_height), 
                 color=colors, rot=30)

    ax.set_ylabel('Metric Value')
    ax.set_xlabel('')
    
    ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.2), 
              ncol=2, frameon=False)

    save_figure("bargraph_complexity")

def plot_complexity_runtime(df_cmplx, df_perf):
    # 1. Merge the two dataframes to align Heuristic with Performance data
    df = df_perf.merge(df_cmplx, on='Engine')

    # 2. Normalisation (Switch-Case baseline)
    # Using 'Switch-Case' as the key. If your CSV uses 'SC', change this string.
    baseline = (
        df[df['Engine'] == 'Switch-Case']
        .set_index('Benchmark')['Time (msec)']
    )

    df['Time_norm'] = df.apply(
        lambda r: r['Time (msec)'] / baseline[r['Benchmark']],
        axis=1
    )

    fig, ax = plt.subplots(figsize=(3.4, 3.4))
    colors = plt.cm.tab10.colors

    markers = {
        'SC': 'o',
        'TC': 's',
        'CG': '^',
        'BD': 'D'
    }

    benchmarks = df['Benchmark'].unique()

    # 3. Plotting per benchmark
    for i, bench in enumerate(benchmarks):
        # Sort by Heuristic so lines connect in correct order
        subset = df[df['Benchmark'] == bench].sort_values('Heuristic')

        # Main continuous line
        ax.plot(
            subset['Heuristic'],
            subset['Time_norm'],
            color=colors[i % len(colors)],
            linewidth=1.2,
            label=bench
        )

        # Overlay markers per engine
        for _, row in subset.iterrows():
            ax.scatter(
                row['Heuristic'],
                row['Time_norm'],
                marker=markers[row['Engine']],
                color=colors[i % len(colors)],
                s=18,
                zorder=3
            )

    ax.set_xlabel("Interpreter Complexity")
    ax.set_ylabel("Runtime (Normalised to Switch-Case)")
    ax.grid(True, linewidth=0.3, alpha=0.5)

    ax.legend(
        loc='upper center',
        bbox_to_anchor=(0.5, 1.25),
        ncol=2,
        fontsize='small',
        frameon=False
    )

    plt.tight_layout()
    plt.show()

# plot graphs
plot_bargraph_runtime(df_performance)
plot_bargraph_instruction_counts(df_efficiency)
plot_bargraph_efficiency(df_efficiency)
plot_stacked_barchart_tmam(df_performance)
plot_bargraph_bad_speculation(df_performance)
plot_bargraph_memory_bound(df_performance)
plot_bargraph_complexity(df_complexity)
#plot_complexity_runtime(df_complexity,
#                        df_performance)
