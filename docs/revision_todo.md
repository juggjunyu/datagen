# SIGGRAPH 2026 Journal Track — Revision TODO

> 状态：Conditionally Accepted  
> 评分：R1(1) R2(-1) R3(3) R4(3)，均分 1.5  
> 本文档基于 Committee Comments、4 位 Reviewer 意见、Rebuttal 承诺及论文实际内容整理

---

## 🔴 硬性接收条件（Committee Required）

- [x] **讨论 Trimming Curves 的处理**（Committee 唯一明确条件）✅ 已完成
  - **修改位置**：`Conclusions.tex` L13-14
  - **内容**：Standard NURBS 转换、simple trims、complex trims、multi-patch models 均已覆盖
  - ~~**注意**：`Related-Work.tex` 中也应补充一句提到 trimmed NURBS 在 CAD 中的普遍性~~ → 不需要，Conclusions 已充分讨论

---

## 🟠 Rebuttal 承诺（必须逐条兑现）

> Committee: "follow up on all promises made in the rebuttal"

### 写作与结构

- [ ] **将 Appendix 关键推导合并进正文** (R3, R4, Committee)
  - **现状**：`Appendices.tex` 有 ~200 行推导（Step I 误差累积、robust implementation、补充公式）
  - **操作**：将 Appendix A（Step I 误差累积）和 Appendix B（robust implementation with `nextafter`）的核心内容合入 `Error-Bound-Analysis.tex` 或 `Error-Bound-Computation.tex`
  - **理由**：R4 说 "appendices are integral to the paper's main theoretical contributions"，journal track 无页数限制

- [x] **添加 "Assumptions / Guarantees" box** (R2) ✅ 已完成
  - **修改位置**：`Error-Bound-Conclusion.tex`，"A Short Summary" 的 itemize 之后
  - **格式**：`\journal{}` + `framed` 环境
  - **内容**：三条假设（线性运动、凸包、IEEE 754）+ 保证（无 FN，可能 FP）+ 不保证（manifold contact、自交、trimmed 域）

- [x] **更新 flow diagram** (R2) ✅ 不需要改
  - Pipeline 全程为 exact rational arithmetic，图中已足够清晰
  - 正文 Section 5.4 和 Verification 段落已明确说明

- [x] **统一术语** (R1) ✅ 已完成
  - 全文 "floating-point reliability" → "floating-point robustness"，"resilient" → "robust"
  - 去掉 "reliability and robustness" 冗余
  - 涉及文件：`CCD.tex`（Abstract）、`Introduction.tex`（L35, L42, L52）、`Results.tex`（L65）
  - 所有改动均用 `\journal{}` 标记

- [x] **首次使用 "dyadic property" 时给出定义** (R1) ✅ 已完成
  - **修改位置**：`Dataset.tex` Section 5.4 L535
  - **已添加**：定义 dyadic rationals = $p/2^k$，IEEE 754 精确表示条件

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

- [x] **浮点精确表示的补充实验** (R1, R4) ✅ 部分完成
  - **已完成**：Section 5.4 展开了 dyadic 性质说明、constraint solving 策略、各子参数约束、Verification 段落（discard rate = 0%）
  - **未完成**：缺少实验表格（生成 N 组数据，报告 `isPreciselyRepresent()` 通过率）

- [ ] **澄清 error decomposition 的 novelty** (R1) → 降级为 nice-to-have
  - **原因**：rebuttal 中是反驳而非承诺，现有正文 `Error-Bound-Analysis.tex` "Core Idea" 小节已阐述 decomposition 思路
  - **如有余力**：可在 L27 前加一句显式对比经典 interval arithmetic 不处理比较分支

### 讨论补充

- [x] **Non-point contacts 讨论** (R1) ✅ 已完成
  - **修改位置**：`Conclusions.tex` L9
  - **已添加**：TDIBM 可处理 manifold contacts，但数据集生成仅支持 isolated points，curve/area contact 需要全新代数设计

- [x] **CAD 适用性路线图** (R2) — 与上方硬性条件合并 ✅ 已完成（见 Conclusions.tex L13-14）

---

## 🟡 逐行修改（Reviewer 指出的具体问题）

### R4 逐行澄清

> 以下行号对应 review 原文。由于 journal revision 内容可能会移位，操作以**语义定位**为准。

- [x] **"parametric surfaces" 含义**（原 Line 66）✅ 已完成
  - Introduction.tex 加 "in this work, we focus on tensor-product and triangular Bézier patches"

- [x] **"collision constraints admit infinitely many solutions"**（原 Line 166）✅ 已完成
  - 改为 "may admit infinitely many solutions due to non-isolated contacts (e.g., along a common curve)"

- [x] **"the surface lies within their convex hull"**（原 Line 186）✅ 跳过
  - 已在 Assumptions/Guarantees box 中正式声明（variation-diminishing property），无需在 Background 重复

- [ ] **定义 $\tau^L$**（原 Line 209）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Background.tex` L68，优先队列排序处
  - **操作**：在 "sorted by ascending $\tau^\mathrm{L}$" 后加括号注释 "(the lower time bound of that subdomain)"

- [x] **"convergence criterion" 细节**（原 Line 211）✅ 已完成
  - Background.tex 加 "(i.e., all parametric intervals have been refined below a user-specified threshold, e.g., $10^{-4}$)"

- [ ] **单区间 vs 多区间**（原 Line 227）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Background.tex` L88 附近
  - **操作**：加脚注 "While multiple disjoint intervals are theoretically possible, storing and intersecting them is significantly more expensive; we adopt a single interval for simplicity as the additional precision gain is marginal."

- [ ] **"non-polygonal" envelope**（原 Line 288）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Error-Bound-Analysis.tex` L8
  - **操作**：在 "non-polygonal geometries" 后加括号 "(e.g., line selection errors may produce self-intersecting or non-convex envelope boundaries that are no longer valid piecewise-linear functions)"

- [ ] **"exact convex hull"**（原 Line 306）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Error-Bound-Analysis.tex` 约 L21
  - **操作**：改为 "assuming the envelope is constructed exactly (i.e., with correct topology and segment ordering), the coefficient error alone..."

- [ ] **$a$ vs $\alpha$ 符号**（原 Line 365）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Error-Bound-Analysis.tex` 或 `Error-Bound-Computation.tex`
  - **操作**：加一句说明 "We use $a$ (Latin) to denote the coefficient values $\{a_\alpha^{(1)}\}$ and $\alpha$ (Greek) as their index."

- [ ] **多线段缺失的累积影响**（原 Line 420）— ⚪ 可做可不做（rebuttal 仅解释，现有正文已有内容）
  - **位置**：`Error-Bound-Computation.tex` 或 `Error-Bound-Intersection.tex`
  - **操作**：加一句 "We first bound the impact of a single segment omission (this subsection), then show that multiple omissions accumulate additively (see Lines XX–XX below)"

- [x] **"assigning compatible values" 详解**（原 Line 576）✅ 已完成
  - Dataset.tex Step II 加 forward reference "(see Section~\ref{sec:principled_design} for details)"
  - Section 5.4 已有详细展开，无需重复

- [ ] **速度采样来源**（原 Line 647）— ⚪ 可做可不做（rebuttal 仅解释）
  - **位置**：`Dataset.tex` L282（corner cases velocity 段落）
  - **操作**：将 "velocities... are sampled within opposite half-spaces induced by the face tangent plane" 改为 "velocities are sampled from the two opposite half-spaces separated by the contact tangent plane (not from the plane itself)"

- [x] **"more false positives" 精确数字**（原 Line 757）✅ 跳过
  - R4 担忧 TDIBM-E 与 TDIBM-H(10⁻¹²) FP 无差异，新表格已通过 10⁻¹⁶/10⁻¹²/10⁻⁸ 三组对比回答了此问题，无需额外文字

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
- [x] ~~手工构造 curve/area contact 示例~~ ✅ 不需要做
  - Rebuttal 说的是 "if required"，Committee 未要求

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
