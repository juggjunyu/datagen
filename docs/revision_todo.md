# SIGGRAPH 2026 Journal Track — Revision TODO

> 状态：Conditionally Accepted  
> 评分：R1(1) R2(-1) R3(3) R4(3)，均分 1.5  
> 本文档基于 Committee Comments、4 位 Reviewer 意见、Rebuttal 承诺及论文实际内容整理

---

## 🔴 硬性接收条件（Committee Required）

- [ ] **讨论 Trimming Curves 的处理**（Committee 唯一明确条件）
  - **修改位置**：`Conclusions.tex`，在现有 limitation 段落后新增一段
  - **写什么**：
    - Standard NURBS → 可无损转为 tensor-product Bézier patches，本文方法原生支持
    - Simple trims（domain clipping）→ 限制参数查询范围即可支持
    - Complex trims → trimming 不影响 3D 求值，难点在参数域裁剪；TDIBM 的递归细分策略可自然扩展（future work）
    - Multi-patch models → 逐 patch 处理 + domain clipping 绕过接缝；self-collision 是 non-trivial future direction
  - **注意**：`Related-Work.tex` 中也应补充一句提到 trimmed NURBS 在 CAD 中的普遍性

---

## 🟠 Rebuttal 承诺（必须逐条兑现）

> Committee: "follow up on all promises made in the rebuttal"

### 写作与结构

- [ ] **将 Appendix 关键推导合并进正文** (R3, R4, Committee)
  - **现状**：`Appendices.tex` 有 ~200 行推导（Step I 误差累积、robust implementation、补充公式）
  - **操作**：将 Appendix A（Step I 误差累积）和 Appendix B（robust implementation with `nextafter`）的核心内容合入 `Error-Bound-Analysis.tex` 或 `Error-Bound-Computation.tex`
  - **理由**：R4 说 "appendices are integral to the paper's main theoretical contributions"，journal track 无页数限制

- [ ] **添加 "Assumptions / Guarantees" box** (R2)
  - **修改位置**：`Error-Bound-Conclusion.tex`（Section "A Short Summary" 附近），或在 `Error-Bound-Analysis.tex` 开头
  - **格式**：用 `framed` 环境包裹，标题 "Assumptions and Guarantees"
  - **内容**：
    - 假设：控制点线性运动、曲面在控制点凸包内（variation-diminishing）、IEEE 754 double
    - 保证：在上述假设下 TDIBM-E 无 FN（可能有 FP）
    - 不保证：manifold contact、自相交、trimmed 参数域

- [ ] **更新 flow diagram** (R2)
  - **修改位置**：`Sections/Figures.tex` 中的 `fig:dataset-pipeline`
  - **操作**：在 pipeline 图中用不同颜色/标注区分 exact rational arithmetic 步骤（Step I-VI）和 floating-point 步骤（最终输出转换）

- [ ] **统一术语** (R1)
  - **问题**：Abstract 和 Introduction 中 "floating-point robustness"、"floating-point reliability"、"floating-point-resilient"、"error-resilient" 混用
  - **操作**：全文搜索替换，统一为 "floating-point robustness"（名词）/ "robust"（形容词），仅在首次出现时解释与 resilience 的关系
  - **涉及文件**：`CCD.tex`（Abstract L140-143）、`Introduction.tex`（L35, L42）
  - 同时统一 "improves"/"mitigates"/"guarantees"：明确 TDIBM-E 是 **消除** FN（guarantee），不是 "降低风险"

- [ ] **首次使用 "dyadic property" 时给出定义** (R1)
  - **修改位置**：`Dataset.tex` Section 5.4（"Construction Design for Floating-Point Representation"，约 L529）
  - **加一句**：定义 dyadic numbers = 分母为 2 的幂次的有理数（即 $p/2^k$），它们可被 IEEE 754 浮点数精确表示

### 实验与分析

- [ ] **为实验数据添加可视化图表** (R1)
  - **现状**：`Results.tex` 中的 `tab:results_updated` 是纯数字表格，18 列难以快速解读
  - **操作**：新增 bar chart 或 grouped bar chart，展示各场景的 FN/FP 数量对比（4 种方法并排）
  - **位置**：`Results.tex` 表格之后，或 `Figures.tex` 中

- [ ] **添加 worst-case runtime 分析** (R2)
  - **现状**：`Results.tex` 只报告了 average runtime
  - **操作**：新增 runtime percentile 图（box plot 或 CDF），展示 P50/P95/P99/max
  - **数据**：rebuttal 中提到 FF worst-case TDIBM-E 48s vs TDIBM-H 40s
  - **位置**：`Results.tex` 或 Appendix 新增一节

- [ ] **浮点精确表示的补充实验** (R1, R4)
  - **现状**：`Dataset.tex` Section 5.4 只说了 "carefully select... to make the generated data exactly representable"，但 R1/R4 都觉得不够清楚
  - **操作**：
    1. 明确报告 discard rate = 0%（因为从 float 起步，保持 dyadic 性质）
    2. 展开 Section 5.4 被注释掉的内容（L548-560 的 `\begin{comment}` 块），解释为什么 Gaussian elimination 可能破坏 dyadic 性质，以及你们如何通过约束法线为 power-of-two 分量来规避
    3. 添加实验表格：生成 N 组数据，报告每组的 `isPreciselyRepresent()` 通过率

- [ ] **澄清 error decomposition 的 novelty** (R1)
  - **修改位置**：`Error-Bound-Analysis.tex` Section "Core Idea for Robust Error Handling"（约 L17-27）
  - **操作**：在现有文字后加一段，明确对比经典 interval arithmetic：
    - 经典方法：operand 误差 → 运算累积 → 结果误差，但不处理 **比较分支**
    - 本文方法：将误差分为 coefficient errors（累积型，标准 interval analysis 可处理）和 arithmetic errors（分支型，比较决策导致拓扑变化，无法累积分析）
    - 核心 novelty：对后者放弃后验修正，转为前置预处理（拉开 envelope），保证即使拓扑变化也是保守的

### 讨论补充

- [ ] **Non-point contacts 讨论** (R1)
  - **修改位置**：`Conclusions.tex`，扩展现有 L8 的 limitation 句子
  - **操作**：目前只有一句 "A limitation of the dataset is its focus on isolated point collisions"，扩展为一段：
    - TDIBM 算法本身可处理 manifold contacts（和单点碰撞方式相同）
    - 数据集生成管线（inverse construction）仅支持 isolated contact points，因为求解器基于 Eq.(1) 的单点约束
    - 生成精确的 curve/area contact 需要根本性的代数设计变更（future work）

- [ ] **CAD 适用性路线图** (R2) — 与上方硬性条件合并

---

## 🟡 逐行修改（Reviewer 指出的具体问题）

### R4 逐行澄清

> 以下行号对应 review 原文。由于 journal revision 内容可能会移位，操作以**语义定位**为准。

- [ ] **"parametric surfaces" 含义**（原 Line 66）
  - **位置**：`CCD.tex` L65（标题）+ `Introduction.tex` 开头
  - **操作**：在 Introduction 首段补充 "In this work, we focus on tensor-product and triangular Bézier patches"

- [ ] **"collision constraints admit infinitely many solutions"**（原 Line 166）
  - **位置**：`Related-Work.tex` L26（CCD dataset 段落）
  - **操作**：改为 "the collision constraints may admit infinitely many solutions due to non-isolated contacts (e.g., along a common curve), causing Mathematica to..."

- [ ] **"the surface lies within their convex hull"**（原 Line 186）
  - **位置**：`Background.tex` L32
  - **操作**：改为 "the surface lies within the convex hull of its control points, a standard consequence of the variation-diminishing property of Bernstein polynomials"

- [ ] **定义 $\tau^L$**（原 Line 209）
  - **位置**：`Background.tex` L68，优先队列排序处
  - **操作**：在 "sorted by ascending $\tau^\mathrm{L}$" 后加括号注释 "(the lower time bound of that subdomain)"

- [ ] **"convergence criterion" 细节**（原 Line 211）
  - **位置**：`Background.tex` L69
  - **操作**：补充 "i.e., the parametric intervals have been refined below a user-specified threshold (e.g., $10^{-4}$)"

- [ ] **单区间 vs 多区间**（原 Line 227）
  - **位置**：`Background.tex` L88 附近
  - **操作**：加脚注 "While multiple disjoint intervals are theoretically possible, storing and intersecting them is significantly more expensive; we adopt a single interval for simplicity as the additional precision gain is marginal."

- [ ] **"non-polygonal" envelope**（原 Line 288）
  - **位置**：`Error-Bound-Analysis.tex` L8
  - **操作**：在 "non-polygonal geometries" 后加括号 "(e.g., line selection errors may produce self-intersecting or non-convex envelope boundaries that are no longer valid piecewise-linear functions)"

- [ ] **"exact convex hull"**（原 Line 306）
  - **位置**：`Error-Bound-Analysis.tex` 约 L21
  - **操作**：改为 "assuming the envelope is constructed exactly (i.e., with correct topology and segment ordering), the coefficient error alone..."

- [ ] **$a$ vs $\alpha$ 符号**（原 Line 365）
  - **位置**：`Error-Bound-Analysis.tex` 或 `Error-Bound-Computation.tex`
  - **操作**：加一句说明 "We use $a$ (Latin) to denote the coefficient values $\{a_\alpha^{(1)}\}$ and $\alpha$ (Greek) as their index."

- [ ] **多线段缺失的累积影响**（原 Line 420）
  - **位置**：`Error-Bound-Computation.tex` 或 `Error-Bound-Intersection.tex`
  - **操作**：加一句 "We first bound the impact of a single segment omission (this subsection), then show that multiple omissions accumulate additively (see Lines XX–XX below)"

- [ ] **"assigning compatible values" 详解**（原 Line 576）
  - **位置**：`Dataset.tex` L53（Step II）
  - **操作**：把 "assign compatible values to the remaining DoFs" 展开为 "randomly generate floating-point-representable coordinates for the control points corresponding to the redundant degrees of freedom, ensuring they satisfy the dyadic constraint described in Section~\ref{sec:principled_design}"

- [ ] **速度采样来源**（原 Line 647）
  - **位置**：`Dataset.tex` L282（corner cases velocity 段落）
  - **操作**：将 "velocities... are sampled within opposite half-spaces induced by the face tangent plane" 改为 "velocities are sampled from the two opposite half-spaces separated by the contact tangent plane (not from the plane itself)"

- [ ] **"more false positives" 精确数字**（原 Line 757）
  - **位置**：`Results.tex` 评估段落
  - **操作**：改为 "with only marginally more false positives (three additional FF and two additional VV cases relative to 175 existing FP)"

### R2 结构建议

- [ ] **Compact notation table**
  - **位置**：`Background.tex` 末尾或 `Error-Bound-Analysis.tex` 开头
  - **内容**：列出 $\epsilon$（machine epsilon）, $\epsilon_a$（coefficient error）, $\epsilon_\mathrm{constr}$（construction error）, $\delta$（envelope pull-apart offset）, $\Delta d$, 迭代上限等

- [ ] **端到端 pseudocode**
  - **位置**：`Error-Bound-Conclusion.tex` 或新增一节
  - **格式**：Algorithm2e 环境，输入=两个 Bézier surfaces + time step，输出=collision/no-collision + ToI
  - **包含**：递归细分 → 误差计算 → 保守比较 → envelope pull-apart → 终止条件

- [ ] **Related work 补充引用** (R2)
  - **位置**：`Related-Work.tex` "Safe CCD" 段落
  - **操作**：补充 robust geometric computation (Shewchuk '96 已有), filtered exact predicates, validated numerics / directed rounding 的引用

- [ ] **区分 "empirically robust" vs "provably guaranteed"** (R2)
  - **位置**：`Related-Work.tex` 和 `Results.tex`
  - **操作**：讨论 TDIBM-H 时明确标注 "empirically robust heuristic"；讨论 TDIBM-E 时标注 "provable guarantee under stated assumptions"

---

## 🔵 排版与格式

- [ ] **修复字体嵌入问题** — PDF 在 Linux 上文字拉伸/乱码 (R4)
  - **操作**：Overleaf 编译设置检查字体嵌入，确保所有字体 embedded subset

- [ ] **Fig 3, 4, 5, 7 文字颜色** (R4)
  - **现状**：使用蓝/绿色文字（`\definecolor{green}{rgb}{0.3,0.56,0.0}` 和 `\color{blue}`）
  - **操作**：改为黑白打印友好的配色（深蓝/深红/黑），或添加不同线型/标记区分
  - **注意**：`CCD.tex` L37 的 `\highlight` 命令和 L47 的 `\journal` 命令都用了 `blue`，如果 camera-ready 需全部改为黑色

- [ ] **Fig 7 左上角 stats** — 文字太小，需放大 (R4)

- [ ] **Table 中 `*` 改为 `\times`** (R4)
  - **现状**：`Results.tex` 的表格中已经用了 `\times`（L490），这个可能已修复
  - **检查**：确认所有表格中的乘号都是 `\times` 而非 `*`

---

## 💡 Nice-to-have（非必须但加分）

- [ ] TDIBM-H 10⁻⁸ tolerance 实验 — **已完成**，表格中已包含 $10^{-8}$, $10^{-12}$, $10^{-16}$ 三组 (R4 ✓)
- [ ] 提交时附 cover letter — 使用 `Sections/Cover-Letter.tex`，说明与 SIGGRAPH Asia 轮次的主要变化 (R4)
- [ ] 手工构造 curve/area contact 示例（如果 Committee 在 camera-ready 阶段要求）(R1)

---

## 建议执行顺序

```
Phase 1 — 内容核心（最高优先，~3 天）
  ├── Trimming curves 讨论 → Conclusions.tex + Related-Work.tex
  ├── Appendix 合入正文 → 重组 Error-Bound 系列 .tex
  ├── Assumptions/Guarantees box → Error-Bound-Analysis.tex
  ├── Flow diagram 更新 → Figures.tex
  └── Section 5.4 展开（dyadic 性质、discard rate）→ Dataset.tex

Phase 2 — 实验补充（~2 天）
  ├── 实验结果可视化（bar chart）→ Results.tex / Figures.tex
  ├── Runtime percentile 图 → Results.tex
  ├── 浮点精确表示实验数据 → Dataset.tex
  └── Error decomposition novelty 段落 → Error-Bound-Analysis.tex

Phase 3 — 逐行修改 + 术语统一（~2 天）
  ├── R4 逐行问题 → Background.tex, Dataset.tex, Error-Bound-*.tex, Results.tex
  ├── R1 术语统一 → CCD.tex (Abstract), Introduction.tex
  ├── R2 notation table + pseudocode → Background.tex / Error-Bound-Conclusion.tex
  └── R2 supplementary references → Related-Work.tex

Phase 4 — 排版收尾 + Cover Letter（~1 天）
  ├── 字体嵌入修复
  ├── 图表颜色 / 文字大小 / 乘号修复
  └── Cover-Letter.tex 撰写
```
