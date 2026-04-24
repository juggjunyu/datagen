"""
Runtime distribution plots for CCD methods (SIGGRAPH 2026 revision).
Two methods side-by-side: each method one row, 6 scenarios as columns.
Two versions: log x-axis and linear x-axis.
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import pandas as pd
import os

# ── Load Linux Libertine font ───────────────────────────────────────────────
_font_dir = r'E:\research\datagen\Libertine Font'
for _f in os.listdir(_font_dir):
    if _f.endswith('.otf') and not _f.startswith('._'):
        fm.fontManager.addfont(os.path.join(_font_dir, _f))
_libertine_name = None
for f in fm.fontManager.ttflist:
    if 'Libertine' in f.name and 'LinLibertine_RZ' in (f.fname or ''):
        _libertine_name = f.name
        break
if _libertine_name is None:
    _libertine_name = 'Linux Libertine'
print(f"Using font: {_libertine_name}")

plt.rcParams.update({
    'font.family': 'serif',
    'font.serif': [_libertine_name, 'Linux Libertine', 'DejaVu Serif'],
    'font.size': 9,
    'axes.labelsize': 10,
    'legend.fontsize': 8,
    'xtick.labelsize': 8,
    'ytick.labelsize': 8,
    'figure.dpi': 300,
    'savefig.bbox': 'tight',
    'savefig.pad_inches': 0.15,
})

SCALE = 2.0

scenarios = ['FF', 'EF', 'EE', 'VF', 'VE', 'VV']
colors = {
    'FF': '#D95319',
    'EF': '#EDB120',
    'EE': '#77AC30',
    'VF': '#4DBEEE',
    'VE': '#7E2F8E',
    'VV': '#A2142F',
}

# ── Methods ─────────────────────────────────────────────────────────────────
methods_list = [
    ('tdibm_h_12', {'label': r'TDIBM-H ($10^{-12}$)',  'path': r'E:\research\datagen\result\tdibm_h_12\timing_detail.csv'}),
    ('tdibm_e',    {'label': 'TDIBM-E (ours)',          'path': r'E:\research\datagen\result\tdibm_e\timing_detail.csv'}),
]

def make_combined_plot(log_scale=True):
    # Load all data, compute global range
    all_data = {}
    global_min, global_max = 1e10, 0
    all_times = []
    for key, info in methods_list:
        df = pd.read_csv(info['path'])
        df['time_ms'] = df['time_sec'] * 1000 * SCALE
        all_data[key] = df
        global_min = min(global_min, max(df['time_ms'].min(), 1e-2))
        global_max = max(global_max, df['time_ms'].max())
        all_times.extend(df['time_ms'].tolist())
    linear_max = np.percentile(all_times, 99)  # unified 99th percentile across all methods

    nrows = len(methods_list)
    ncols = len(scenarios)
    fig, axes = plt.subplots(nrows, ncols, figsize=(16, 2.8 * nrows))

    for row, (key, info) in enumerate(methods_list):
        df = all_data[key]
        for col, scenario in enumerate(scenarios):
            ax = axes[row, col]
            data = df[df['category'] == scenario]['time_ms'].values

            if log_scale:
                bins = np.logspace(np.log10(global_min), np.log10(global_max), 50)
                ax.set_xscale('log')
                ax.set_xlim(global_min * 0.5, global_max * 2)
            else:
                bins = np.linspace(0, linear_max, 50)
                ax.set_xlim(0, linear_max * 1.05)

            ax.hist(data, bins=bins, color=colors[scenario], edgecolor='black',
                    linewidth=0.3, alpha=0.85, zorder=3)
            ax.grid(axis='y', alpha=0.3, zorder=0)
            ax.set_axisbelow(True)

            # Title: scenario name on top row only
            if row == 0:
                ax.set_title(scenario, fontsize=12, pad=6)

            # Y label: method name on leftmost column only
            if col == 0:
                ax.set_ylabel(info['label'], fontsize=10)
            else:
                ax.set_ylabel('')

            # X label: bottom row only
            if row == nrows - 1:
                ax.set_xlabel('Runtime (ms)', fontsize=9)
            else:
                ax.set_xlabel('')

            # Stats box
            stats = (
                f"n={len(data)}\n"
                f"avg={np.mean(data):.1f}\n"
                f"med={np.median(data):.1f}\n"
                f"P95={np.percentile(data, 95):.1f}\n"
                f"max={np.max(data):.1f}"
            )
            ax.text(0.97, 0.95, stats, transform=ax.transAxes,
                    fontsize=6.5, family=_libertine_name,
                    verticalalignment='top', horizontalalignment='right',
                    bbox=dict(boxstyle='round,pad=0.2', facecolor='white', alpha=0.8, edgecolor='gray'))

    # After drawing all histograms, unify y-axis across ALL subplots
    ymax = max(axes[r, c].get_ylim()[1] for r in range(nrows) for c in range(ncols))
    for r in range(nrows):
        for c in range(ncols):
            axes[r, c].set_ylim(0, ymax * 1.05)

    xtype = 'log' if log_scale else 'linear'
    plt.subplots_adjust(left=0.05, right=0.98, top=0.92, bottom=0.10, wspace=0.25, hspace=0.30)
    out = rf'E:\research\datagen\scripts\runtime_distribution_combined_{xtype}.pdf'
    fig.savefig(out)
    plt.close(fig)
    print(f"Saved {os.path.basename(out)}")

# ── Generate ────────────────────────────────────────────────────────────────
make_combined_plot(log_scale=True)
make_combined_plot(log_scale=False)

# ── Combined figure: each subplot has linear main + log inset ────────────────
def make_overview_plot():
    # Load all data
    all_data = {}
    global_min, global_max = 1e10, 0
    all_times = []
    for key, info in methods_list:
        df = pd.read_csv(info['path'])
        df['time_ms'] = df['time_sec'] * 1000 * SCALE
        all_data[key] = df
        global_min = min(global_min, max(df['time_ms'].min(), 1e-2))
        global_max = max(global_max, df['time_ms'].max())
        all_times.extend(df['time_ms'].tolist())
    linear_max = np.percentile(all_times, 99)

    nrows = len(methods_list)
    ncols = len(scenarios)
    fig, axes = plt.subplots(nrows, ncols, figsize=(16, 3.0 * nrows))

    # First pass: draw all histograms
    for row, (key, info) in enumerate(methods_list):
        df = all_data[key]
        for col, scenario in enumerate(scenarios):
            ax = axes[row, col]
            data = df[df['category'] == scenario]['time_ms'].values

            # Main linear histogram
            bins_lin = np.linspace(0, linear_max, 50)
            ax.hist(data, bins=bins_lin, color=colors[scenario], edgecolor='black',
                    linewidth=0.3, alpha=0.85, zorder=3)
            ax.set_xlim(0, linear_max * 1.05)
            ax.grid(axis='y', alpha=0.3, zorder=0)
            ax.set_axisbelow(True)

            if row == 0:
                ax.set_title(scenario, fontsize=12, pad=6)
            if col == 0:
                ax.set_ylabel(info['label'], fontsize=10)
            else:
                ax.set_ylabel('')
            if row == nrows - 1:
                ax.set_xlabel('Runtime (ms)', fontsize=9)
            else:
                ax.set_xlabel('')

            # Stats text in lower-right (avoid overlapping inset in upper-right)
            stats = (
                f"n={len(data)}  avg={np.mean(data):.1f}\n"
                f"P95={np.percentile(data, 95):.1f}  max={np.max(data):.1f}"
            )
            ax.text(0.97, 0.25, stats, transform=ax.transAxes,
                    fontsize=9.5, family=_libertine_name,
                    verticalalignment='bottom', horizontalalignment='right',
                    bbox=dict(boxstyle='round,pad=0.2', facecolor='white', alpha=0.8, edgecolor='gray'))

            # Log inset in upper-right corner of each subplot
            inset = ax.inset_axes([0.52, 0.45, 0.46, 0.50])  # [x0, y0, width, height] in axes coords
            bins_log = np.logspace(np.log10(global_min), np.log10(global_max), 30)
            inset.hist(data, bins=bins_log, color=colors[scenario], edgecolor='black',
                       linewidth=0.2, alpha=0.85)
            inset.set_xscale('log')
            inset.set_xlim(global_min * 0.5, global_max * 2)
            inset.tick_params(labelsize=5, length=2, pad=1)
            inset.grid(axis='y', alpha=0.2)
            inset.set_axisbelow(True)
            inset.patch.set_alpha(0.9)

    # Unify y-axis globally for main plots
    ymax = max(axes[r, c].get_ylim()[1] for r in range(nrows) for c in range(ncols))
    for r in range(nrows):
        for c in range(ncols):
            axes[r, c].set_ylim(0, ymax * 1.05)

    plt.subplots_adjust(left=0.05, right=0.98, top=0.92, bottom=0.08, wspace=0.25, hspace=0.30)
    out = r'E:\research\datagen\scripts\runtime_distribution_overview.pdf'
    fig.savefig(out)
    plt.close(fig)
    print(f"Saved {os.path.basename(out)}")

make_overview_plot()
