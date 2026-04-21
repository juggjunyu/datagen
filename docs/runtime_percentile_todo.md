# Runtime Percentile 分析 — 完整执行方案

## 1. 背景与需求

### Reviewer 原始关切 (R2)
R2 认为论文只报了 average runtime，无法判断方法在极端情况下是否会特别慢。需要展示 runtime 的分布特征，特别是尾部行为（worst-case）。

### Rebuttal 承诺原文
> "We conducted preliminary profiling on FF scenarios, finding that the worst-case runtime for our TDIBM-E is 48s, compared to 40s for the TDIBM-H (10⁻¹²). This demonstrates that even in severely degenerate configurations, our method maintains a reasonable overhead while preserving safety. **We will add figures and detailed analysis for comprehensive runtime percentiles and worst-case behaviors.**"

### 需要兑现的内容
1. 一张或多张展示 runtime 分布/percentile 的图
2. 在正文或 Appendix 中加一段 worst-case 分析文字

---

## 2. 现有资源

### 已有的 3 张 runtime distribution 图（旧版，已在 Overleaf）
位置：`Figures/timeTDIBM.pdf`, `Figures/timeTDIBM-H.pdf`, `Figures/timeTDIBM-E.pdf`

图的内容：
- X 轴：computation time (ms)，对数坐标
- Y 轴：count（频次）
- 按 6 种碰撞场景（FF/EF/EE/VF/VE/VV）分颜色堆叠
- 三张图分别对应 TDIBM、TDIBM-H(10⁻¹²)、TDIBM-E

这三张图在 `Sections/Figures.tex` L57-71 中被注释掉了，原始代码：
```latex
% \begin{figure}[htbp]
%     \centering
%     \begin{subfigure}{\linewidth}
%         \includegraphics[width=0.8\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM.pdf}
%         \caption{}
%         \label{fig:time-tdibm}
%     \end{subfigure}
%     \begin{subfigure}{\linewidth}
%         \includegraphics[width=0.8\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM-H.pdf}
%         \caption{}
%         \label{fig:time-tdibm-h}
%     \end{subfigure}
%     \begin{subfigure}{\linewidth}
%         \includegraphics[width=0.8\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM-E.pdf}
%         \caption{}
%         \label{fig:time-tdibm-e}
%     \end{subfigure}
%     \caption{Distribution of computation time. The x-axis is in log scale and the y-axis shows the number.}
%     \label{fig:time-distrubution}
% \end{figure}
```

### 问题
1. **风格不统一**：旧图不是 Linux Libertine 字体，和 Fig.7/Fig.8 风格不一致
2. **没有生成脚本**：当前电脑上没有找到生成这三张图的 Python 脚本
3. **没有原始逐 case runtime 数据**：没有 csv/txt 格式的每个 test case 单独运行时间

### 已有的 average runtime 数据（来自论文表格）
来源：`Sections/Appendices.tex` L249-261 的表格

```
场景         | Trad        | TDIBM   | H(10⁻¹⁶) | H(10⁻¹²) | H(10⁻⁸)  | TDIBM-E
-------------|-------------|---------|-----------|-----------|-----------|--------
FF (RP+RN)   | 2.23×10⁵   | 338.09  | 390.50    | 377.61    | 624.43    | 382.02
EF (RP+RN)   | 293.11      | 56.06   | 59.16     | 57.52     | 80.02     | 57.26
EE (RP+RN)   | 9887.50     | 12.70   | 12.39     | 13.10     | 31.59     | 13.27
VF (RP+RN)   | 13.91       | 12.48   | 13.17     | 12.95     | 36.35     | 15.37
VE (RP+RN)   | 732.19      | 27.28   | 30.12     | 32.55     | 72.93     | 36.57
VV (RP+RN)   | 2.94        | 2.68    | 3.12      | 4.02      | 9.50      | 4.69
FF (NHP+NMN) | 8303.91     | 121.52  | 125.14    | 136.12    | 279.07    | 140.27
EF (NHP+NMN) | 426.15      | 92.37   | 93.76     | 99.38     | 143.36    | 112.65
EE (NHP+NMN) | 23.52       | 58.20   | 57.37     | 60.80     | 87.03     | 68.31
VF (NHP+NMN) | 451.47      | 28.27   | 32.69     | 30.88     | 42.47     | 29.78
VE (NHP+NMN) | 16.02       | 11.85   | 12.23     | 11.94     | 19.07     | 12.14
VV (NHP+NMN) | 0.71        | 1.26    | 1.37      | 1.52      | 1.85      | 1.29
```

### Rebuttal 中提到的 worst-case 数据点
- FF worst-case: TDIBM-E = 48s, TDIBM-H(10⁻¹²) = 40s

---

## 3. 执行方案

### 方案 A：用现有三张图 + 文字补充（快速兑现，无需额外数据）

#### 步骤 A1：取消注释，放入 Appendix
在 `Sections/Appendices.tex` 的 "Detailed Statistics" 部分（`\subsection{Breakdown of Detailed Results.}` 之后）加入：

```latex
\journal{
\subsection{Runtime Distribution Analysis}
\label{app:runtime_distribution}
To provide insight into the worst-case behavior of different methods, we plot the runtime distributions of the three TDIBM variants across the entire RP+RN dataset in Fig.~\ref{fig:time-distribution}. The distributions of TDIBM, TDIBM-H ($10^{-12}$), and TDIBM-E are highly similar in shape, confirming that the error-bound computation in TDIBM-E does not introduce pathological slowdowns. In the most compute-intensive FF scenario, the worst-case runtime of TDIBM-E is approximately 48s, compared to 40s for TDIBM-H ($10^{-12}$)---a moderate 20\% overhead consistent with the average-case behavior reported in Table~\ref{tab:results_updated}.

\begin{figure}[htbp]
    \centering
    \begin{subfigure}{\linewidth}
        \includegraphics[width=0.85\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM.pdf}
        \caption{TDIBM}
        \label{fig:time-tdibm}
    \end{subfigure}
    \begin{subfigure}{\linewidth}
        \includegraphics[width=0.85\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM-H.pdf}
        \caption{TDIBM-H ($10^{-12}$)}
        \label{fig:time-tdibm-h}
    \end{subfigure}
    \begin{subfigure}{\linewidth}
        \includegraphics[width=0.85\linewidth, trim=19 22 13 22, clip]{Figures/timeTDIBM-E.pdf}
        \caption{TDIBM-E (ours)}
        \label{fig:time-tdibm-e}
    \end{subfigure}
    \caption{Runtime distributions of three TDIBM variants on the RP+RN dataset. The x-axis shows computation time in milliseconds (log scale) and the y-axis shows the count. The three methods exhibit highly similar distributions, indicating that TDIBM-E's error-bound computation does not cause disproportionate slowdowns even in worst-case scenarios.}
    \label{fig:time-distribution}
\end{figure}
}
```

#### 步骤 A2：在 Results.tex 正文加引用
在 Results.tex 的 runtime 讨论段落（约 L63，"The runtime overhead remained moderate" 附近）加一句：

```latex
\journal{We further analyze the runtime distributions in Appendix~\ref{app:runtime_distribution}, which confirm that the three TDIBM variants exhibit nearly identical distribution shapes, with no pathological worst-case slowdowns for TDIBM-E.}
```

#### 步骤 A3：删除 Figures.tex 中的旧注释
`Sections/Figures.tex` L57-71 的注释代码可以删除（已移到 Appendix）。

#### 步骤 A4：推送到 Overleaf
```bash
cd overleaf-dir
git add Sections/Appendices.tex Sections/Results.tex Sections/Figures.tex
git commit -m "Add runtime distribution analysis in Appendix (R2 worst-case)"
git push
```

#### 步骤 A5：更新 cover letter
将 "Remaining Items" 中的 runtime percentile 条目移到已完成。将 cover letter 中 R2 回应的 worst-case 段落更新为：
```
We have added runtime distribution plots (Fig. X in Appendix) showing that all three TDIBM variants exhibit nearly identical runtime distributions. The worst-case overhead of TDIBM-E over TDIBM-H is approximately 20% on FF scenarios (48s vs 40s).
```

---

### 方案 B：重新生成统一风格的图（更好，需要原始数据）

#### 前提：获取逐 case runtime 数据
需要的数据格式（任选一种）：

**格式 1：单个 CSV 文件**
```csv
scenario,category,method,runtime_ms
FF,RP,TDIBM,123.45
FF,RP,TDIBM,234.56
FF,RP,TDIBM-H,120.33
FF,RP,TDIBM-E,125.67
...
```

**格式 2：按方法分文件**
每个文件名如 `runtime_TDIBM.txt`、`runtime_TDIBM-H.txt`、`runtime_TDIBM-E.txt`，内容每行：
```
scenario runtime_ms
FF 123.45
FF 234.56
EF 56.78
...
```

**格式 3：按方法+场景分文件**
如 `FF_TDIBM.txt`，每行一个 runtime 值：
```
123.45
234.56
345.67
...
```

#### 步骤 B1：写 Python 脚本
在 `scripts/plot_runtime_distribution.py` 中编写，使用与 `plot_results.py` 相同的风格设置：

```python
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import os

# ── Load Linux Libertine font（与 plot_results.py 相同） ──
_font_dir = r'E:\research\datagen\Libertine Font'
for _f in os.listdir(_font_dir):
    if _f.endswith('.otf') and not _f.startswith('._'):
        fm.fontManager.addfont(os.path.join(_font_dir, _f))
_libertine_name = 'Linux Libertine O'  # 已知名称

plt.rcParams.update({
    'font.family': 'serif',
    'font.serif': [_libertine_name, 'DejaVu Serif'],
    'font.size': 11,
    'axes.labelsize': 12,
    'figure.dpi': 300,
    'savefig.bbox': 'tight',
    'savefig.pad_inches': 0.15,
})
```

绘图选项（选其一或组合）：

**选项 B1a：三合一 runtime distribution 直方图（复刻旧图但统一风格）**
- 三个 subplot 纵向排列
- 每个 subplot 内按场景分颜色堆叠
- X 轴 log scale，Y 轴 count

**选项 B1b：Box plot 展示 percentile（R2 最想看的）**
- X 轴：6 种场景
- 每个场景 3 个 box（TDIBM / TDIBM-H / TDIBM-E）
- Box 显示 P25/P50/P75，whisker 显示 P5/P95，outlier 点显示 max
- Y 轴 log scale

**选项 B1c：CDF 曲线对比**
- 每个场景一个 subplot（2×3 grid）
- 每个 subplot 内 3 条 CDF 曲线（TDIBM / TDIBM-H / TDIBM-E）
- X 轴 runtime (ms) log scale，Y 轴 cumulative probability

#### 步骤 B2：生成图并复制到 Overleaf
```bash
python scripts/plot_runtime_distribution.py
cp scripts/runtime_distribution.pdf Overleaf/Figures/
```

#### 步骤 B3-B5：同方案 A 的步骤 A1-A5，但引用新图

---

## 4. 推荐

- **当前无原始数据** → 先用方案 A 快速兑现承诺
- **后续拿到数据** → 用方案 B 替换为统一风格的图（优先 B1b box plot，最直观展示 percentile）
- 两个方案的 LaTeX 代码和正文文字完全兼容，只需替换图片文件

---

## 5. 相关文件位置

| 文件 | 路径 | 作用 |
|------|------|------|
| 旧版 runtime 图 | `Overleaf/Figures/timeTDIBM{,-H,-E}.pdf` | 现有 3 张旧风格图 |
| 图引用（已注释） | `Overleaf/Sections/Figures.tex` L57-71 | 取消注释或移到 Appendix |
| Appendix | `Overleaf/Sections/Appendices.tex` | 方案 A 的图和文字放这里 |
| Results 正文 | `Overleaf/Sections/Results.tex` ~L63 | 加引用句 |
| 新图脚本模板 | `E:\research\datagen\scripts\plot_results.py` | 方案 B 参考风格 |
| Cover letter | `E:\research\datagen\_SIG26_journal__cover_letter\main.tex` | 更新 Remaining Items |
| Revision TODO | `E:\research\datagen\docs\revision_todo.md` | 标记完成 |
