# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**subdiv-ccd** — A C++ data generator for Continuous Collision Detection (CCD) benchmarks on triangular quadratic Bézier surfaces. It generates precise collision/near-miss/near-hit training data using GMP rational arithmetic to guarantee IEEE 754 double-precision exact representability.

## Build & Run

Build system: **xmake** (not CMake). Toolchain: MinGW (gcc/g++).

```bash
# Build (debug by default)
xmake

# Build release
xmake f -m release && xmake

# Run generator
xmake r generator -g <workType> -t <taskType> -o <outputDir>

# Direct binary (needed for parallel runs to avoid xmake lock conflicts)
build/windows/x64/release/generator.exe -g 1 -t 2 -o output
```

### CLI Arguments

| Flag | Description | Values |
|------|-------------|--------|
| `-g` | Work mode | 0=test, 1=single generation, 2=batch process folders, 3=restore data, 5=batch seed from file |
| `-t` | Task type (collision type) | 0=FF, 1=EF, 2=EE, 3=VF, 4=VE, 5=VV, 6-11=NearMiss variants, 12-17=NearHit variants |
| `-o` | Output directory prefix | Auto-appends `_seed{value}` |
| `-i` | Input file/directory | Seed file for mode 5, folder for mode 2 |

### Parallel Data Generation

Use `parallel_generator.py` for multi-process batch generation (avoids xmake lock issues by calling the binary directly). Configure the script header variables: `EXE_PATH`, `TASK_TYPE`, `TARGET_COUNT`, `WORKER_COUNT`, `FINAL_OUTPUT_DIR`.

```bash
python parallel_generator.py
```

Success detection: stdout contains "no collision" → data is valid.

## Architecture

### Dependencies (via xmake)

- **GMP** — arbitrary-precision rational arithmetic (linked from `gmp/` local dir)
- **Eigen** — linear algebra (Vector3r, Array2r)
- **nlohmann_json**, **spdlog**, **fmt**

### Build Targets

1. **rational** (static lib) — `rational-cpp/src/` — GMP-based Rational number wrapper
2. **ccd_io** (static lib) — `CCD-Query-IO/src/` — CCD query CSV/JSON I/O (depends on rational)
3. **generator** (binary) — `generator/` — main data generator (depends on ccd_io)

### Core Source Files (`generator/`)

| File | Role |
|------|------|
| `main.cpp` | Entry point, arg parsing, animation sequence generation |
| `generator.cpp` | Core collision generators: `generateFF`, `generateEE`, `generateEF`, `generateVF`, `generateVE`, `generateVV`, NearMiss/NearHit variants, `saveControlPointsData` |
| `generator.h` | Function declarations, `generateVelocityField`, `isPreciselyRepresent` |
| `gentype.h` | Data structures (`CollisionPoint`, `BaryCoord`), helper functions, velocity generators (`genColVel`, `generateVelocityFieldIndependent`) |
| `gentype.cpp` | `generateVelocityFieldIndependent` implementation |
| `triBezier.h` | `TriQuadBezier` class — 6-control-point triangular quadratic Bézier surface with `evaluatePatchPoint`, `evaluateNormal`, `divideBezierPatch` |
| `config.h` | Type aliases (`Rational`, `Vector3r`, `Array2r`), global constants |
| `collisionDetector.h` | Collision verification |
| `paramBound.h` | Parameter domain bounds for subdivision |
| `paramMesh.h` | Mesh generation and OBJ export |

### Key Data Types (defined in `config.h`)

```cpp
using Rational = rational::Rational;          // GMP arbitrary-precision rational
using Vector3r = Eigen::Matrix<Rational, 3, 1>;
using Array2r  = Eigen::Array<Rational, 2, 1>;
```

### Critical Design Invariant: Float-Exact Representability

All output control point coordinates must be exactly representable as IEEE 754 `double`. This requires:
1. **Denominator must be a power of 2** (so it has a finite binary representation)
2. **Numerator must fit in 53 bits** (mantissa width of double)

Implementation strategy:
- Random numbers generated as `float` (23-bit mantissa), not `double`, to keep initial numerators small
- All operations (Bézier evaluation, de Casteljau subdivision) only use +, ×, linear interpolation → denominators stay as powers of 2
- `SCALE_FACTOR = 1/131072 (2^{-17})` preserves the power-of-2 denominator
- `isPreciselyRepresent()` validates via round-trip: `Rational → double → Rational`, checking equality

### Data Generation Pipeline

1. Generate random UV parameters on triangle boundary edges
2. Generate two random Bézier patches (6 control points each)
3. Translate patch1 so collision points coincide at the given UVs
4. Generate separation velocities (in respective normal half-spaces)
5. Generate independent velocity fields for all control points
6. Subdivide patches locally around collision point
7. Save as CSV: `x_num, x_den, y_num, y_den, z_num, z_den, ground_truth` (24 rows per query)

### Output Format

CSV with 24 rows per collision query:
- Rows 1-6: Patch1 start positions (t=1)
- Rows 7-12: Patch2 start positions (t=1)
- Rows 13-18: Patch1 end positions (t=0)
- Rows 19-24: Patch2 end positions (t=0)

## Code Style Preferences

- 优先保证代码简洁易懂，不要过度设计
- 注意圈复杂度，函数尽量小、可复用
- 修改前做最小化修改，尽量不影响其他模块
- 解释代码时说人话，最好配 mermaid 图（暗黑主题兼容的颜色）
- 修 Bug 时遵循：理解 → 分析（至少两种可能原因）→ 制定计划 → 请求确认 → 执行 → 审查 → 解释
- 改动后，假定 10 条 case 输入并给出预期结果
