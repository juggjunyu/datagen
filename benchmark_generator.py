#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据生成器基准测试脚本
用于测试 xmake r generator -t 0~11 每种任务类型的平均生成时间
"""

import subprocess
import time
import re
import shutil
import tempfile
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Optional


# ========== 配置区域 ==========
TASK_TYPES = range(0, 12)  # 任务类型 0-11
SUCCESS_COUNT_TARGET = 10  # 每种类型需要成功的次数
MAX_ATTEMPTS_PER_TYPE = 100000  # 每种类型最大尝试次数（防止无限循环）
TIMEOUT = 600  # 单次运行超时时间（秒）
USE_XMAKE = True  # 是否使用 xmake r generator 命令
# 如果 USE_XMAKE = False，则使用以下路径
EXE_PATH = "E:\\research\\datagen\\build\\windows\\x64\\debug\\generator1.exe"
WORK_DIR = "E:\\research\\datagen"  # xmake 工作目录
# ==============================


@dataclass
class RunResult:
    """单次运行结果"""
    success: bool
    elapsed_time: float  # 秒
    seed: Optional[str] = None
    reason: Optional[str] = None


@dataclass
class TaskTypeStats:
    """单个任务类型的统计信息"""
    task_type: int
    total_time: float = 0.0  # 总耗时（包含失败的时间）
    success_count: int = 0  # 成功次数
    total_attempts: int = 0  # 总尝试次数
    runs: List[RunResult] = field(default_factory=list)  # 详细运行记录
    
    @property
    def success_rate(self) -> float:
        """成功率 = 成功次数 / 总尝试次数"""
        if self.total_attempts == 0:
            return 0.0
        return self.success_count / self.total_attempts * 100
    
    @property
    def avg_time_per_success(self) -> float:
        """平均每个成功数据所需时间 = 总时间 / 成功次数（核心指标）"""
        if self.success_count == 0:
            return 0.0
        return self.total_time / self.success_count
    
    @property
    def min_success_time(self) -> float:
        """成功运行中的最小单次时间"""
        success_times = [r.elapsed_time for r in self.runs if r.success]
        return min(success_times) if success_times else 0.0
    
    @property
    def max_success_time(self) -> float:
        """成功运行中的最大单次时间"""
        success_times = [r.elapsed_time for r in self.runs if r.success]
        return max(success_times) if success_times else 0.0


def run_generator(task_type: int, use_xmake: bool = True) -> RunResult:
    """
    运行一次生成器并记录时间
    
    Args:
        task_type: 任务类型 (0-11)
        use_xmake: 是否使用 xmake 命令
    
    Returns:
        RunResult: 运行结果
    """
    # 创建临时目录作为输出目录
    temp_dir = Path(tempfile.mkdtemp(prefix=f"bench_t{task_type}_"))
    output_path = temp_dir / "benchmark_output"
    
    try:
        if use_xmake:
            cmd = ["xmake", "r", "generator", "-t", str(task_type), "-o", str(output_path)]
            cwd = WORK_DIR
        else:
            cmd = [EXE_PATH, "-t", str(task_type), "-o", str(output_path)]
            cwd = temp_dir
        
        # 记录开始时间
        start_time = time.perf_counter()
        
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            encoding='utf-8',
            timeout=TIMEOUT
        )
        
        # 记录结束时间
        elapsed_time = time.perf_counter() - start_time
        
        stdout = result.stdout or ""
        
        # 检查是否成功
        has_success_marker = "no collision" in stdout
        seed_match = re.search(r"seed:\s*(\d+)", stdout)
        seed = seed_match.group(1) if seed_match else None
        
        if has_success_marker and seed:
            return RunResult(
                success=True,
                elapsed_time=elapsed_time,
                seed=seed
            )
        else:
            reason = "missing_success_marker" if not has_success_marker else "missing_seed"
            return RunResult(
                success=False,
                elapsed_time=elapsed_time,
                reason=reason
            )
    
    except subprocess.TimeoutExpired:
        elapsed_time = TIMEOUT
        return RunResult(
            success=False,
            elapsed_time=elapsed_time,
            reason="timeout"
        )
    
    except Exception as exc:
        return RunResult(
            success=False,
            elapsed_time=0.0,
            reason=f"exception: {exc}"
        )
    
    finally:
        # 清理临时目录
        try:
            if temp_dir.exists():
                shutil.rmtree(temp_dir)
        except Exception:
            pass


def run_benchmark() -> List[TaskTypeStats]:
    """
    运行完整的基准测试
    
    新逻辑：对每种任务类型，一直运行直到成功 SUCCESS_COUNT_TARGET 次，
    然后用 总时间 / 成功次数 来计算平均每个成功数据所需时间。
    
    Returns:
        List[TaskTypeStats]: 每种任务类型的统计信息
    """
    all_stats: List[TaskTypeStats] = []
    
    print("=" * 70)
    print("数据生成器基准测试")
    print("=" * 70)
    print(f"任务类型范围: {min(TASK_TYPES)} - {max(TASK_TYPES)}")
    print(f"每种类型目标成功次数: {SUCCESS_COUNT_TARGET}")
    print(f"每种类型最大尝试次数: {MAX_ATTEMPTS_PER_TYPE}")
    print(f"使用命令: {'xmake r generator' if USE_XMAKE else EXE_PATH}")
    print("计时方式: 总时间(含失败) / 成功次数 = 平均每个成功数据耗时")
    print("=" * 70)
    print()
    
    overall_start_time = time.time()
    
    for task_type in TASK_TYPES:
        stats = TaskTypeStats(task_type=task_type)
        print(f">>> 测试任务类型 {task_type} (目标: 成功 {SUCCESS_COUNT_TARGET} 次):")
        
        # 开始计时（包含所有尝试的时间）
        type_start_time = time.perf_counter()
        
        attempt = 0
        while stats.success_count < SUCCESS_COUNT_TARGET and attempt < MAX_ATTEMPTS_PER_TYPE:
            attempt += 1
            
            result = run_generator(task_type, USE_XMAKE)
            stats.runs.append(result)
            stats.total_attempts += 1
            
            if result.success:
                stats.success_count += 1
                status = f"✓ 成功 [{stats.success_count}/{SUCCESS_COUNT_TARGET}]"
            else:
                status = f"✗ 失败({result.reason})"
            
            print(f"    [尝试 {attempt}] {status} "
                  f"本次耗时: {result.elapsed_time:.3f}s")
        
        # 结束计时
        stats.total_time = time.perf_counter() - type_start_time
        
        # 输出该类型的统计
        if stats.success_count > 0:
            print(f"    --- 类型 {task_type} 小结: "
                  f"成功 {stats.success_count}/{stats.total_attempts}次尝试, "
                  f"成功率 {stats.success_rate:.1f}%, "
                  f"总耗时 {stats.total_time:.3f}s, "
                  f"平均每个成功数据耗时 {stats.avg_time_per_success:.3f}s ---")
        else:
            print(f"    --- 类型 {task_type} 小结: "
                  f"尝试 {stats.total_attempts} 次均失败 ---")
        print()
        
        all_stats.append(stats)
    
    overall_elapsed = time.time() - overall_start_time
    
    # 打印最终汇总表格
    print_summary(all_stats, overall_elapsed)
    
    return all_stats


def print_summary(all_stats: List[TaskTypeStats], total_time: float):
    """打印汇总报告"""
    print()
    print("=" * 70)
    print("基准测试汇总报告")
    print("=" * 70)
    print("计时方式: 总时间(含失败) / 成功次数 = 平均每个成功数据耗时")
    print()
    
    # 表头
    header = (
        f"{'类型':^6} | {'成功/尝试':^10} | {'成功率':^8} | "
        f"{'总耗时':^10} | {'平均耗时/成功':^14}"
    )
    print(header)
    print("-" * 70)
    
    total_success = 0
    total_attempts = 0
    total_all_time = 0.0
    
    for stats in all_stats:
        total_success += stats.success_count
        total_attempts += stats.total_attempts
        total_all_time += stats.total_time
        
        success_str = f"{stats.success_count}/{stats.total_attempts}"
        rate_str = f"{stats.success_rate:.1f}%"
        total_time_str = f"{stats.total_time:.3f}s"
        
        if stats.success_count > 0:
            avg_str = f"{stats.avg_time_per_success:.3f}s"
        else:
            avg_str = "N/A"
        
        print(f"{stats.task_type:^6} | {success_str:^10} | {rate_str:^8} | "
              f"{total_time_str:^10} | {avg_str:^14}")
    
    print("-" * 70)
    
    # 总体统计
    overall_rate = (total_success / total_attempts * 100) if total_attempts > 0 else 0
    overall_avg = total_all_time / total_success if total_success > 0 else 0
    
    success_str = f"{total_success}/{total_attempts}"
    rate_str = f"{overall_rate:.1f}%"
    total_time_str = f"{total_all_time:.3f}s"
    avg_str = f"{overall_avg:.3f}s" if total_success > 0 else "N/A"
    
    print(f"{'总计':^6} | {success_str:^10} | {rate_str:^8} | "
          f"{total_time_str:^10} | {avg_str:^14}")
    
    print()
    print(f"测试总耗时: {total_time:.2f}s ({total_time/60:.2f}分钟)")
    print("=" * 70)
    
    # 额外输出：用于报告的关键指标
    print()
    print("【关键指标 - 可用于报告】")
    print(f"  • 平均生成一个成功数据所需时间: {overall_avg:.3f} 秒/个")
    print(f"  • 总体成功率: {overall_rate:.1f}%")
    print(f"  • 预估生成速度: {3600/overall_avg:.1f} 个/小时" if overall_avg > 0 else "  • 生成速度: N/A")
    print()
    
    # 按类型列出平均时间
    print("【各类型平均生成时间（总时间/成功次数）】")
    for stats in all_stats:
        if stats.success_count > 0:
            print(f"  • 类型 {stats.task_type:2d}: {stats.avg_time_per_success:.3f}s "
                  f"(成功率: {stats.success_rate:.1f}%, "
                  f"尝试 {stats.total_attempts} 次成功 {stats.success_count} 次)")
        else:
            print(f"  • 类型 {stats.task_type:2d}: 无成功数据 (尝试 {stats.total_attempts} 次)")


def main():
    """主函数"""
    try:
        run_benchmark()
    except KeyboardInterrupt:
        print("\n\n用户中断测试")


if __name__ == "__main__":
    main()
