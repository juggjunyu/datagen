"""
Generate bar charts for SIGGRAPH 2026 revision.
Reads data from the paper's Table (Appendices.tex) and produces:
  (a) FN rate comparison across scenarios
  (b) Average runtime comparison across scenarios
Output: two PDF files for inclusion in the paper.
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import os

# ── Load Linux Libertine font ───────────────────────────────────────────────
_font_dir = r'E:\research\datagen\Libertine Font'
for _f in os.listdir(_font_dir):
    if _f.endswith('.otf') and not _f.startswith('._'):
        fm.fontManager.addfont(os.path.join(_font_dir, _f))
# Find the registered family name
_libertine_name = None
for f in fm.fontManager.ttflist:
    if 'Libertine' in f.name and 'LinLibertine_RZ' in (f.fname or ''):
        _libertine_name = f.name
        break
if _libertine_name is None:
    _libertine_name = 'Linux Libertine'
print(f"Using font: {_libertine_name}")

# ── Data from Table (RP+RN rows only, the main benchmark) ──────────────────

scenarios = ['FF', 'EF', 'EE', 'VF', 'VE', 'VV']

# Total samples per scenario (denominator in FN/FP fractions)
# Trad has fewer samples for FF (20), EE (10) due to timeout
total = {
    'Trad':      [20,   1020, 10,   1000, 1000, 1000],
    'TDIBM':     [6003, 1020, 1000, 1000, 1000, 1000],
    'H_16':      [6003, 1020, 1000, 1000, 1000, 1000],
    'H_12':      [6003, 1020, 1000, 1000, 1000, 1000],
    'H_8':       [6003, 1020, 1000, 1000, 1000, 1000],
    'TDIBM-E':   [6003, 1020, 1000, 1000, 1000, 1000],
}

# FN counts
fn = {
    'Trad':      [0,  0,  0,   12,  73,  0],
    'TDIBM':     [0,  0,  135, 212, 452, 860],
    'H_16':      [0,  0,  20,  22,  196, 658],
    'H_12':      [0,  0,  0,   0,   0,   0],
    'H_8':       [0,  0,  0,   0,   0,   0],
    'TDIBM-E':   [0,  0,  0,   0,   0,   0],
}

# FP counts
fp = {
    'Trad':      [13, 2,  7,  0,   363, 65],
    'TDIBM':     [6,  1,  50, 0,   223, 44],
    'H_16':      [6,  1,  50, 0,   223, 44],
    'H_12':      [6,  1,  50, 0,   223, 44],
    'H_8':       [9,  1,  50, 0,   223, 44],
    'TDIBM-E':   [6,  1,  50, 0,   223, 44],
}

# Average runtime (ms)
runtime = {
    'Trad':      [2.23e5, 293.11,  9887.50, 13.91,  732.19, 2.94],
    'TDIBM':     [338.09, 56.06,   12.70,   12.48,  27.28,  2.68],
    'H_16':      [390.50, 59.16,   12.39,   13.17,  30.12,  3.12],
    'H_12':      [377.61, 57.52,   13.10,   12.95,  32.55,  4.02],
    'H_8':       [624.43, 80.02,   31.59,   36.35,  72.93,  9.50],
    'TDIBM-E':   [382.02, 57.26,   13.27,   15.37,  36.57,  4.69],
}

# ── Plot settings ───────────────────────────────────────────────────────────

# Only show methods with meaningful differences for FN chart
# Trad has too few samples in FF/EE, skip it for cleaner presentation
fn_methods = ['TDIBM', 'H_16', 'H_12', 'TDIBM-E']
fn_labels  = ['TDIBM', r'TDIBM-H ($10^{-16}$)', r'TDIBM-H ($10^{-12}$)', 'TDIBM-E (ours)']
fn_colors  = ['#4DBEEE', '#EDB120', '#77AC30', '#D95319']
fn_hatches = ['', '', '', '']

# For runtime chart, show all including Trad and H(10^-8) to show the range
rt_methods = ['Trad', 'TDIBM', 'H_16', 'H_12', 'H_8', 'TDIBM-E']
rt_labels  = ['Trad.', 'TDIBM', r'H ($10^{-16}$)', r'H ($10^{-12}$)', r'H ($10^{-8}$)', 'TDIBM-E (ours)']
rt_colors  = ['#A2142F', '#4DBEEE', '#EDB120', '#77AC30', '#7E2F8E', '#D95319']

plt.rcParams.update({
    'font.family': 'serif',
    'font.serif': [_libertine_name, 'Linux Libertine', 'DejaVu Serif'],
    'font.size': 9,
    'axes.labelsize': 10,
    'legend.fontsize': 7.5,
    'xtick.labelsize': 9,
    'ytick.labelsize': 8,
    'figure.dpi': 300,
    'savefig.bbox': 'tight',
    'savefig.pad_inches': 0.15,
})

# ── Figure (a): FN rate ─────────────────────────────────────────────────────

fig, ax = plt.subplots(figsize=(5.5, 3.0))

x = np.arange(len(scenarios))
n = len(fn_methods)
width = 0.18
offsets = np.arange(n) - (n - 1) / 2

for i, (method, label, color) in enumerate(zip(fn_methods, fn_labels, fn_colors)):
    rates = [fn[method][j] / total[method][j] * 100 for j in range(len(scenarios))]
    bars = ax.bar(x + offsets[i] * width, rates, width * 0.9,
                  label=label, color=color, edgecolor='black', linewidth=0.5,
                  hatch=fn_hatches[i], zorder=3)
    # Add count labels on bars with FN > 0
    for xi, (rate, count) in enumerate(zip(rates, fn[method])):
        if count > 0:
            ax.text(x[xi] + offsets[i] * width, rate + 1.2, f'{count}',
                    ha='center', va='bottom', fontsize=6.5)

ax.set_ylabel('False Negative Rate (%)')
ax.set_xticks(x)
ax.set_xticklabels(scenarios)
ax.set_ylim(0, 100)
ax.legend(loc='upper left', ncol=2, framealpha=0.9)
ax.grid(axis='y', alpha=0.3, zorder=0)
ax.set_axisbelow(True)

fig.subplots_adjust(left=0.10, right=0.97, top=0.88, bottom=0.13)
fig.savefig(r'E:\research\datagen\scripts\fn_comparison.pdf')
plt.close(fig)
print("Saved fn_comparison.pdf")

# ── Figure (b): Runtime (log scale) ────────────────────────────────────────

fig, ax = plt.subplots(figsize=(5.5, 3.0))

x = np.arange(len(scenarios))
n = len(rt_methods)
width = 0.13
offsets = np.arange(n) - (n - 1) / 2

for i, (method, label, color) in enumerate(zip(rt_methods, rt_labels, rt_colors)):
    vals = runtime[method]
    bars = ax.bar(x + offsets[i] * width, vals, width * 0.9,
                  label=label, color=color, edgecolor='black', linewidth=0.5, zorder=3)

ax.set_ylabel('Average Runtime (ms)')
ax.set_xticks(x)
ax.set_xticklabels(scenarios)
ax.set_yscale('log')
ax.set_ylim(1, 5e5)
ax.legend(loc='upper right', ncol=2, framealpha=0.9, fontsize=6.5)
ax.grid(axis='y', alpha=0.3, zorder=0)
ax.set_axisbelow(True)

plt.tight_layout()
fig.savefig(r'E:\research\datagen\scripts\runtime_comparison.pdf')
plt.close(fig)
print("Saved runtime_comparison.pdf")

# ── Combined figure (a+b side by side) ──────────────────────────────────────

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(8.0, 4.0))

# (a) FN rate
x = np.arange(len(scenarios))
n = len(fn_methods)
width = 0.18
offsets = np.arange(n) - (n - 1) / 2

for i, (method, label, color) in enumerate(zip(fn_methods, fn_labels, fn_colors)):
    rates = [fn[method][j] / total[method][j] * 100 for j in range(len(scenarios))]
    bars = ax1.bar(x + offsets[i] * width, rates, width * 0.9,
                   label=label, color=color, edgecolor='black', linewidth=0.5,
                   zorder=3)
    for xi, (rate, count) in enumerate(zip(rates, fn[method])):
        if count > 0:
            ax1.text(x[xi] + offsets[i] * width, rate + 1.5, f'{count}',
                     ha='center', va='bottom', fontsize=9)

ax1.set_ylabel('False Negative Rate (%)', fontsize=12)
ax1.set_xticks(x)
ax1.set_xticklabels(scenarios, fontsize=11)
ax1.set_ylim(0, 100)
ax1.tick_params(axis='y', labelsize=10)
ax1.set_title('(a) False Negatives', fontsize=13, pad=8)
ax1.legend(loc='upper left', ncol=1, framealpha=0.9, fontsize=9, handlelength=1.5)
ax1.grid(axis='y', alpha=0.3, zorder=0)
ax1.set_axisbelow(True)

# (b) Runtime
n2 = len(rt_methods)
width2 = 0.13
offsets2 = np.arange(n2) - (n2 - 1) / 2

for i, (method, label, color) in enumerate(zip(rt_methods, rt_labels, rt_colors)):
    vals = runtime[method]
    ax2.bar(x + offsets2[i] * width2, vals, width2 * 0.9,
            label=label, color=color, edgecolor='black', linewidth=0.5, zorder=3)

ax2.set_ylabel('Average Runtime (ms)', fontsize=12)
ax2.set_xticks(x)
ax2.set_xticklabels(scenarios, fontsize=11)
ax2.set_yscale('log')
ax2.set_ylim(1, 5e5)
ax2.tick_params(axis='y', labelsize=10)
ax2.set_title('(b) Average Runtime', fontsize=13, pad=8)
ax2.legend(loc='upper right', ncol=1, framealpha=0.9, fontsize=8.5, handlelength=1.5)
ax2.grid(axis='y', alpha=0.3, zorder=0)
ax2.set_axisbelow(True)

plt.subplots_adjust(left=0.08, right=0.97, top=0.88, bottom=0.12, wspace=0.32)
fig.savefig(r'E:\research\datagen\scripts\results_combined.pdf')
plt.close(fig)
print("Saved results_combined.pdf")
