import numpy as np
import matplotlib.pyplot as plt
import os
from scipy.stats import gaussian_kde

def plot_float_distribution(filename, bins=50, log_scale=None, clip_outliers=False, clip_percent=1):
    """
    从文本文件读取浮点数并绘制其分布图，可自动调整轴的分布
    
    参数:
        filename: 包含浮点数的文本文件路径
        bins: 直方图的分箱数量
        log_scale: 自动或手动设置对数坐标轴 ('auto', 'x', 'y', 'both', None)
        clip_outliers: 是否裁剪异常值
        clip_percent: 异常值裁剪的百分比（两侧各去除的百分比）
    """
    # 检查文件是否存在
    if not os.path.exists(filename):
        print(f"错误: 文件 '{filename}' 不存在!")
        return
    
    try:
        # 从文本文件读取浮点数
        with open(filename, 'r') as file:
            data = []
            for line in file:
                # 去除空白字符并按空格分割每行
                values = line.strip().split()
                # 尝试将每个值转换为浮点数
                for val in values:
                    try:
                        data.append(float(val))
                    except ValueError:
                        print(f"警告: 无法转换值 '{val}' 为浮点数，已忽略")
        
        if not data:
            print("警告: 文件中没有有效的浮点数")
            return
        
        # 最简洁的方式
        data = [float(line.split()[1]) for line in open(filename)]
        
        # 转换为numpy数组以便于分析
        data = np.array(data)
        
        # 裁剪异常值（如果需要）
        # if clip_outliers and len(data) > 10:
        #     lower_bound = np.percentile(data, clip_percent)
        #     upper_bound = np.percentile(data, 100 - clip_percent)
        #     data_clipped = data[(data >= lower_bound) & (data <= upper_bound)]
            
        #     # 如果裁剪后的数据量太少，则使用原始数据
        #     if len(data_clipped) < len(data) * 0.5:
        #         print("警告: 裁剪异常值后数据量太少，使用原始数据")
        #     else:
        #         print(f"信息: 已裁剪异常值（{clip_percent}%），从 {len(data)} 个点减少到 {len(data_clipped)} 个点")
        #         data = data_clipped

        # 自动确定是否应该使用对数刻度
        data_range = max(data) / (min(data) if min(data) > 0 else min(filter(lambda x: x > 0, data)))
        
        if log_scale == 'auto':
            use_log_x = False
            use_log_y = False
            
            # 计算数据分布范围，如果数据跨度超过3个数量级，考虑使用对数刻度
            if data_range > 1000:
                # 创建两个图表，对比线性和对数刻度的效果
                hist_linear, bin_edges = np.histogram(data, bins=bins)
                hist_count_range = max(hist_linear) / (min(hist_linear) if min(hist_linear) > 0 else 1)
                
                if data_range > 10000 or hist_count_range > 100:
                    use_log_x = True
        else:
            use_log_x = log_scale in ['x', 'both']
            use_log_y = log_scale in ['y', 'both']
        
        # 创建分布图
        plt.figure(figsize=(10, 8))
        
        # 绘制直方图
        if use_log_x:
            # 对数刻度的直方图需要对数间隔的bins
            min_val = min(data)
            if min_val <= 0:  # 对数刻度不能处理负数或零
                min_val = min(filter(lambda x: x > 0, data))
            
            log_bins = np.logspace(np.log10(min_val), np.log10(max(data)), bins)
            counts, edges, bars = plt.hist(data, bins=log_bins, alpha=0.7, color="#6EC2E6", 
                                          edgecolor='black', linewidth=0.7, label='Distribution of computation time')
            plt.xscale('log')
        else:
            counts, edges, bars = plt.hist(data, bins=bins, alpha=0.7, color='#6EC2E6', 
                                          edgecolor='black', linewidth=0.7, label='Distribution of computation time')
        
        # 添加核密度估计曲线
        try:
            if use_log_x:
                # 直接在线性数据上计算，但使用更合适的缩放
                kde = gaussian_kde(data)
                kde_xs = np.logspace(np.log10(min_val), np.log10(max(data)), 1000)
                kde_values = kde(kde_xs)
                
                # 使用直方图的峰值位置进行缩放匹配
                hist_max_idx = np.argmax(counts)
                hist_peak_pos = (log_bins[hist_max_idx] + log_bins[hist_max_idx + 1]) / 2
                kde_peak_value = kde(hist_peak_pos)
                
                if kde_peak_value > 0:
                    scale_factor = counts[hist_max_idx] / kde_peak_value
                    kde_scaled = kde_values * scale_factor
                else:
                    kde_scaled = kde_values * (max(counts) / max(kde_values))
                
                # plt.plot(kde_xs, kde_scaled, color='red', linewidth=2, label='密度估计')
            else:
                # 线性刻度处理保持不变
                kde = gaussian_kde(data)
                kde_xs = np.linspace(min(data), max(data), 1000)
                kde_values = kde(kde_xs)
                bin_width = (max(data) - min(data)) / bins
                kde_scaled = kde_values * len(data) * bin_width
                
                # plt.plot(kde_xs, kde_scaled, color='red', linewidth=2, label='密度估计')
                        
        except Exception as e:
            print(f"警告: 无法生成密度估计: {e}")
        
        # 如果需要应用对数刻度到Y轴
        if use_log_y:
            plt.yscale('log')
        
        # 添加标签和标题
        plt.xlabel('value')
        plt.ylabel('num')
        title = 'Distribution of Computation Time'
        if use_log_x:
            title += ' (X-axis Log Scale)'
        if use_log_y:
            title += ' (Y-axis Log Scale)'
        plt.title(title)
        plt.grid(True, alpha=0.3)
        plt.legend()
        
        # 添加统计信息文本
        stats_text = (
            f"numbers: {len(data)}\n"
            f"min: {min(data):.4g}\n"
            f"max: {max(data):.4g}\n"
            f"avg: {np.mean(data):.4g}\n"
            f"mid: {np.median(data):.4g}\n"
            f"sigma: {np.std(data):.4g}\n"
            # f"range: {data_range:.2g}倍"
        )
        plt.figtext(0.15, 0.70, stats_text, bbox=dict(facecolor='white', alpha=0.5))
        
        # 保存图像
        plt.tight_layout()
        output_filename = os.path.splitext(filename)[0] + '_distribution.pdf'
        plt.savefig(output_filename, format='pdf', bbox_inches='tight')
        print(f"分布图已保存为: {output_filename}")
        
        # 显示图像
        plt.show()
        
        # 如果数据分布很广，自动创建对数刻度的版本
        if not use_log_x and data_range > 1000:
            print("提示: 检测到数据分布跨度很大，自动创建对数刻度版本...")
            plot_float_distribution(filename, bins=bins, log_scale='x', clip_outliers=clip_outliers)
            
    except Exception as e:
        print(f"错误: {e}")

# 使用示例
if __name__ == "__main__":
    # 将这里的文件名替换为你的txt文件路径
    input_file = "robust_newer/timelog.txt"
    
    # 自动检测是否应该使用对数刻度
    plot_float_distribution(input_file, bins=50, log_scale='auto', clip_outliers=True, clip_percent=1)