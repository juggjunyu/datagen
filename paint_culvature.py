#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
paint_culvature.py - 绘制RMS曲率分布柱状图
输入: txt文件，每行2个数字，代表RMS曲率
输出: 对数坐标的柱状图，显示频率分布和统计信息
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import os

# ── Load Linux Libertine font ───────────────────────────────────────────────
_font_dir = r'E:\research\datagen\Libertine Font'
if os.path.isdir(_font_dir):
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


def read_curvature_data(filepath):
    """
    读取曲率数据文件
    每行2个数字，空格分隔
    返回所有曲率值的列表
    """
    curvatures = []
    with open(filepath, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            for val in parts:
                try:
                    curvatures.append(float(val))
                except ValueError:
                    continue
    return np.array(curvatures)


def trim_outliers(data, percentile=1.0):
    """
    去除数据的最大和最小前 percentile% 的异常值
    
    参数:
        data: numpy数组，原始数据
        percentile: 要去除的百分比（默认1.0，即去除前后各1%）
    
    返回:
        过滤后的数据，保留中间 (100 - 2*percentile)% 的数据
    """
    lower_bound = np.percentile(data, percentile)
    upper_bound = np.percentile(data, 100 - percentile)
    
    # 保留在 [lower_bound, upper_bound] 范围内的数据
    filtered_data = data[(data >= lower_bound) & (data <= upper_bound)]
    
    removed_count = len(data) - len(filtered_data)
    print(f"去除异常值: 移除了 {removed_count} 个点 (前后各{percentile}%)")
    print(f"  - 下界(P{percentile}): {lower_bound:.4g}")
    print(f"  - 上界(P{100-percentile}): {upper_bound:.4g}")
    
    return filtered_data


def plot_curvature_distribution(curvatures, output_path=None, trim_percent=1.0):
    """
    绘制曲率分布柱状图
    X轴使用对数坐标
    显示统计信息

    参数:
        curvatures: 曲率数据数组
        output_path: 输出图像路径（可选）
        trim_percent: 去除前后异常值的百分比（默认1.0%）
    """
    # 过滤掉非正数（对数坐标需要正数）
    curvatures = curvatures[curvatures > 0]
    original_count = len(curvatures)

    # 去除前后 trim_percent% 的异常值
    if trim_percent > 0:
        curvatures = trim_outliers(curvatures, trim_percent)

    # 计算统计信息
    count = len(curvatures)
    min_val = np.min(curvatures)
    max_val = np.max(curvatures)
    avg_val = np.mean(curvatures)
    mid_val = np.median(curvatures)
    sigma_val = np.std(curvatures)

    # ── Style: match plot_results.py ──
    plt.rcParams.update({
        'font.family': 'serif',
        'font.serif': [_libertine_name, 'Linux Libertine', 'DejaVu Serif'],
        'font.size': 11,
        'axes.labelsize': 13,
        'legend.fontsize': 11,
        'xtick.labelsize': 11,
        'ytick.labelsize': 11,
        'figure.dpi': 300,
        'savefig.bbox': 'tight',
        'savefig.pad_inches': 0.05,
    })

    # 创建图形
    fig, ax = plt.subplots(figsize=(5.5, 3.2))

    # 使用对数空间的bins
    log_min = np.log10(min_val)
    log_max = np.log10(max_val)
    bins = np.logspace(log_min, log_max, 40)

    # 绘制柱状图 — 与 plot_results.py 配色一致
    n, bins_out, patches = ax.hist(curvatures, bins=bins,
                                    color='#4DBEEE',
                                    edgecolor='black',
                                    linewidth=0.5,
                                    alpha=0.9,
                                    zorder=3)

    # 设置X轴为对数坐标
    ax.set_xscale('log')

    # 设置标签
    ax.set_xlabel('RMS Curvature')
    ax.set_ylabel('Count')

    # 添加统计信息文本框 — 放大字体
    stats_text = (f'Total: {original_count}\n'
                  f'Shown: {count}\n'
                  f'Min: {min_val:.4g}\n'
                  f'Max: {max_val:.4g}\n'
                  f'Mean: {avg_val:.4g}\n'
                  f'Median: {mid_val:.4g}\n'
                  f'Std: {sigma_val:.4g}')

    # 文本框样式
    props = dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='gray')
    ax.text(0.97, 0.97, stats_text, transform=ax.transAxes, fontsize=12,
            verticalalignment='top', horizontalalignment='right', bbox=props,
            family=_libertine_name)

    # 设置网格 — 与 plot_results.py 一致
    ax.grid(True, alpha=0.3, which='both', zorder=0)
    ax.set_axisbelow(True)

    # 调整布局
    plt.tight_layout()
    
    # 保存或显示
    if output_path:
        # 保存PDF格式（矢量图，适合论文）
        pdf_path = output_path.rsplit('.', 1)[0] + '.pdf'
        plt.savefig(pdf_path, format='pdf', bbox_inches='tight')
        print(f"PDF已保存到: {pdf_path}")

        # 也保存PNG格式
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"图像已保存到: {output_path}")

    plt.show()
    
    # 打印统计信息
    print("\n=== 统计信息 ===")
    print(f"原始数据点: {original_count}")
    print(f"过滤后数据点: {count}")
    print(f"最小值: {min_val:.4g}")
    print(f"最大值: {max_val:.4g}")
    print(f"平均值: {avg_val:.4g}")
    print(f"中位数: {mid_val:.4g}")
    print(f"标准差: {sigma_val:.4g}")


def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python paint_culvature.py <输入文件.txt> [输出图像.png]")
        print("示例: python paint_culvature.py culvature.txt output.png")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    print(f"读取文件: {input_file}")
    curvatures = read_curvature_data(input_file)
    print(f"共读取 {len(curvatures)} 个曲率值")
    
    # 默认去除前后各1%的异常值
    plot_curvature_distribution(curvatures, output_file, trim_percent=1.0)


if __name__ == "__main__":
    main()
