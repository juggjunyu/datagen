# 实验方案：完整重跑 + Runtime Percentile 分析

## 1. 背景

### 为什么要重跑
- 当前 `result/` 文件夹的数据（td, td_with_trick, trad_10）和论文表格 **不是同一版数据集**
  - result: FF=11972, EF=2000, EE=1818, VF=2118, VE=2200, VV=2314
  - 论文表: FF=6603, EF=1120, EE=1100, VF=1100, VE=1100, VV=1100（仅 RP+RN 部分）
- trad 只跑了 20 个样本，无法用于论文
- result 里缺少 TDIBM-E、TDIBM-H(10⁻¹⁶)、TDIBM-H(10⁻⁸) 的结果
- 需要保证 **正文表、附录表、Fig.8 柱状图、runtime 分布图** 四者使用完全相同的一次实验结果

### Reviewer 需求 (R2)
Rebuttal 承诺："We will add figures and detailed analysis for comprehensive runtime percentiles and worst-case behaviors."

---

## 2. 完整数据集规模

根据附录 Table (tab:generation_statistics)，完整数据集包含 4 类样本：

| 场景 | RP (GT=1) | RN (GT=0) | NHP (GT=1) | NMN (GT=0) | Total |
|------|-----------|-----------|------------|------------|-------|
| FF   | 6003      | 6003      | 600        | 600        | 13206 |
| EF   | 1020      | 1020      | 100        | 100        | 2240  |
| EE   | 1000      | 1000      | 100        | 100        | 2200  |
| VF   | 1000      | 1000      | 100        | 100        | 2200  |
| VE   | 1000      | 1000      | 100        | 100        | 2200  |
| VV   | 1000      | 1000      | 100        | 100        | 2200  |
| **合计** | **11023** | **11023** | **1100** | **1100** | **24246** |

- **RP**: Regular Positive，真实碰撞
- **RN**: Regular Negative，缩短时间区间后无碰撞（与 RP 一一对应）
- **NHP**: Near-Hit Positive，擦边碰撞
- **NMN**: Near-Miss Negative，擦边不碰

### 论文中两张表的分法

**正文表 (tab:results_updated)**：合并 RP+RN+NHP+NMN，例如：
- FF total = 6003(RP) + 600(NHP) = 6603 (碰撞类，分母用于 FN)
- FF total = 6003(RN) + 600(NMN) = 6603 (非碰撞类，分母用于 FP)
- 注意：FF 正文表的分母是 6603 而非 13206，因为 FN 只对碰撞类有意义，FP 只对非碰撞类有意义

**附录表 (tab:results)**：RP+RN 和 NHP+NMN 分开两半报告

---

## 3. 数据集文件结构

数据集在另一台电脑 `D:\projects\CCDdataset\ours\` 下：

```
ours/
├── face-face/
│   ├── dataset_typeFaceFace.csv        # RP, 每 24 行 = 1 个 query
│   ├── fp_dataset_typeFaceFace.csv     # RN
│   ├── nearhit_typeFaceFace.csv        # NHP (文件名待确认)
│   └── nearmiss_typeFaceFace.csv       # NMN (文件名待确认)
├── edge-face/   (同结构)
├── edge-edge/   (同结构)
├── vert-face/   (同结构)
├── vert-edge/   (同结构)
└── vert-vert/   (同结构)
```

> ⚠️ NHP/NMN 文件名待确认。到了另一台电脑先 `ls` 看实际文件名。

### 验证数据集版本

```bash
# 每 24 行 = 1 个 query，数数是否和上面的表一致
for f in ours/*/dataset_*.csv; do echo "$f: $(($(wc -l < "$f") / 24)) queries"; done
for f in ours/*/fp_dataset_*.csv; do echo "$f: $(($(wc -l < "$f") / 24)) queries"; done
```

期望结果：
- face-face/dataset: 6003 queries (RP)
- face-face/fp_dataset: 6003 queries (RN)
- 其他场景: 1000-1020 queries

---

## 4. 需要跑的方法及配置

共 **6 个方法**（对应正文 Table 的 6 列）：

| 编号 | 方法 | 配置说明 |
|------|------|----------|
| 1 | Trad. | 传统 inclusion-based CCD，**全量跑**（之前只跑了 20 个，远远不够） |
| 2 | TDIBM | 基础版 TDIBM，无 envelope pulling |
| 3 | TDIBM-H (10⁻¹⁶) | heuristic pull-apart, factor = 1e-16 |
| 4 | TDIBM-H (10⁻¹²) | heuristic pull-apart, factor = 1e-12 |
| 5 | TDIBM-H (10⁻⁸) | heuristic pull-apart, factor = 1e-8 |
| 6 | TDIBM-E | error-bound 方法 (ours) |

每个方法跑 **全部 6 个场景 × 4 类样本**：

| 场景 | RP 条数 | RN 条数 | NHP 条数 | NMN 条数 | 每方法总 queries |
|------|---------|---------|----------|----------|-----------------|
| FF   | 6003    | 6003    | 600      | 600      | 13206           |
| EF   | 1020    | 1020    | 100      | 100      | 2240            |
| EE   | 1000    | 1000    | 100      | 100      | 2200            |
| VF   | 1000    | 1000    | 100      | 100      | 2200            |
| VE   | 1000    | 1000    | 100      | 100      | 2200            |
| VV   | 1000    | 1000    | 100      | 100      | 2200            |
| **合计** | | | | | **24246** |

6 个方法 × 24246 queries = **145,476 次 CCD 测试**

### ⚠️ Trad. 方法特别注意

- Trad. 在 FF 上特别慢（平均 151 秒/query），6003 RP + 6003 RN = 12006 queries 约需 **20+ 天**
- 之前论文只跑了 20 个样本有理由的
- 建议：
  - 如果附录表的 Trad. FF 条目可以保持 20 样本（`13/20`, `0/20`），就不重跑 FF Trad.
  - 或者设超时限制（如 600s/query），超时算 timeout 跳过
  - NHP+NMN 的 Trad. FF 也是超时问题（附录表用 `---` 标注）
- **其他场景的 Trad. 速度还行**，正常跑

---

## 5. 输出目录结构

```
result_final/
├── trad/
│   ├── total.txt           # FN/FP 汇总 + total cases + total time
│   ├── timelog.txt          # 逐 case: "case_index runtime_seconds"
│   └── log.txt             # FP/FN 发生的 case 编号
├── tdibm/
├── tdibm_h_16/
├── tdibm_h_12/
├── tdibm_h_8/
└── tdibm_e/
    ├── total.txt
    ├── timelog.txt
    └── log.txt
```

### timelog.txt 格式（已有格式，保持不变）

```
case_index runtime_seconds
0 0.0284784000000000
1 0.0878558000000000
...
```

case 的排列顺序：`FF_RP → FF_RN → EF_RP → EF_RN → ... → VV_RP → VV_RN → FF_NHP → FF_NMN → ... → VV_NHP → VV_NMN`

> ⚠️ 排列顺序待确认。用 timelog 行数和 total cases 各场景之和交叉验证。

### total.txt 格式（已有格式，保持不变）

```
FN FF:0
FP FF:175
FN EF:0
FP EF:7
...
total cases FF:6603
total cases EF:1120
...
total time FF:327.73
total time EF:57.76
...
```

> ⚠️ 确认 total.txt 是只报 RP+RN 还是全部 4 类。附录表需要 RP+RN 和 NHP+NMN 分开，所以最好分开报或在 log.txt 里能区分。

---

## 6. 执行步骤

### Step 1：确认数据集
```bash
# 确认文件存在和 query 数量
for dir in face-face edge-face edge-edge vert-face vert-edge vert-vert; do
    echo "=== $dir ==="
    for f in ours/$dir/*.csv; do
        lines=$(wc -l < "$f")
        echo "  $(basename $f): $lines lines = $((lines/24)) queries"
    done
done
```

### Step 2：跑 6 个方法

具体命令取决于 CCD 测试程序的接口，根据实际情况填入。

每个方法跑完后立刻检查 `total.txt`：
```bash
# 快速检查 FN/FP
cat result_final/tdibm/total.txt
cat result_final/tdibm_e/total.txt
```

### Step 3：核对 FN/FP

FN/FP 是确定性的。核对表（RP+RN 部分，正文表分母）：

| 场景 | 指标 | Trad. | TDIBM | H(16) | H(12) | H(8) | TDIBM-E |
|------|------|-------|-------|-------|-------|------|---------|
| FF | FP | 13/40* | 175/6603 | 175/6603 | 175/6603 | 178/6603 | 175/6603 |
| FF | FN | 0/20* | 0/6603 | 0/6603 | 0/6603 | 0/6603 | 0/6603 |
| EF | FP | 9/1120 | 7/1120 | 7/1120 | 7/1120 | 7/1120 | 7/1120 |
| EF | FN | 0/1120 | 0/1120 | 0/1120 | 0/1120 | 0/1120 | 0/1120 |
| EE | FP | 96/110 | 121/1100 | 121/1100 | 121/1100 | 121/1100 | 121/1100 |
| EE | FN | 0/110 | 135/1100 | 20/1100 | 0/1100 | 0/1100 | 0/1100 |
| VF | FP | 1/1100 | 0/1100 | 0/1100 | 0/1100 | 0/1100 | 0/1100 |
| VF | FN | 12/1100 | 212/1100 | 22/1100 | 0/1100 | 0/1100 | 0/1100 |
| VE | FP | 458/1100 | 310/1100 | 310/1100 | 312/1100 | 312/1100 | 310/1100 |
| VE | FN | 73/1100 | 452/1100 | 196/1100 | 0/1100 | 0/1100 | 0/1100 |
| VV | FP | 132/1100 | 56/1100 | 56/1100 | 56/1100 | 58/1100 | 56/1100 |
| VV | FN | 0/1100 | 860/1100 | 658/1100 | 0/1100 | 0/1100 | 0/1100 |

> *Trad. 的 FF 分母不同是因为只跑了部分样本

**如果 FN/FP 不一致，说明数据集版本不对，停下来排查。**

### Step 4：拷贝结果
将 `result_final/` 拷到 `E:\research\datagen\result_final\`。

### Step 5：画图 & 更新论文

在这台电脑上：

1. **更新 Fig.8**：修改 `scripts/plot_results.py` 中的 runtime 数据，重新生成 `results_combined.pdf`
2. **画 runtime 分布图**：新建 `scripts/plot_runtime_distribution.py`
3. **更新论文表格**：runtime 平均值可能略有波动，用新数据统一更新
4. **Appendix 加 runtime 分布图 + 文字**
5. **Results.tex 加引用**
6. **更新 cover letter**

---

## 7. Runtime 分布图方案选择

| 方案 | 图类型 | 优点 | 推荐 |
|------|--------|------|------|
| A: Box plot | X轴=场景, 每场景3-6个box, Y轴log | 直观展示 P25/P50/P75/P95/max | ✅ **首选** |
| B: 直方图 | 3 subplot 纵排 | 展示完整分布形状 | 作为补充 |
| C: CDF 曲线 | 2×3 grid，每格多条线 | 精确读取任意 percentile | 信息密度高 |

推荐 A 放 Appendix，最直接回应 R2 "worst-case percentile" 关切。

---

## 8. Checklist

- [ ] 确认数据集文件齐全，query 数量与论文一致
- [ ] 确认 NHP/NMN 文件名
- [ ] 确认 timelog.txt 中 case 的排列顺序（哪个场景先，RP/RN 怎么交替）
- [ ] 跑 6 个方法（Trad 注意 FF 超时问题）
- [ ] 核对 FN/FP 与上表一致
- [ ] 拷贝 result_final/ 到本机
- [ ] 更新 `plot_results.py` 数据，重新生成 Fig.8
- [ ] 编写 `plot_runtime_distribution.py`，生成 runtime 分布图
- [ ] 更新论文正文表 runtime 数据（如有变化）
- [ ] Appendix 加入 runtime 分布 subsection
- [ ] Results.tex 加引用
- [ ] 更新 cover letter "Remaining Items"
