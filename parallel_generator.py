#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
多进程并行数据生成脚本
用于并行运行 C++ 数据生成器，充分利用多核 CPU 加速数据生成
"""

import os
import sys
import shutil
import tempfile
import subprocess
import re
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path

# ========== 配置区域 ==========
EXE_PATH = "E:\\research\\datagen\\build\\windows\\x64\\debug\\generator1.exe"  # 二进制文件路径
OUTPUT_DIR_NAME = "Face-Face"  # 输出目录名称（可包含相对路径）
TASK_TYPE = 0  # 任务类型，0-11，None 表示不指定
TARGET_COUNT = 4000  # 目标数据组数
WORKER_COUNT = 16  # 并行进程数，None 表示自动设置为 cpu_count - 2
FINAL_OUTPUT_DIR = "output_data_Face-Face"  # 最终输出目录
TIMEOUT = 300  # 子进程超时时间（秒）
# ==============================


def run_generator_task(task_id, exe_path, output_dir_name, task_type=None, timeout=TIMEOUT):
    """在独立临时目录中运行生成器并返回执行结果详情。"""

    temp_work_dir = Path(tempfile.mkdtemp(prefix=f"gen_task_{task_id}_"))

    # 确保输出目录位于临时目录下，避免跨任务冲突
    requested_output = Path(output_dir_name)
    if requested_output.is_absolute():
        requested_output = Path(requested_output.name)
    worker_output_base = temp_work_dir / requested_output
    worker_output_base.parent.mkdir(parents=True, exist_ok=True)

    cmd = [exe_path, "-o", str(worker_output_base)]
    if task_type is not None:
        cmd.extend(["-t", str(task_type)])

    try:
        result = subprocess.run(
            cmd,
            cwd=temp_work_dir,
            capture_output=True,
            text=True,
            encoding='utf-8',
            timeout=timeout
        )
    except subprocess.TimeoutExpired as exc:
        return {
            "success": False,
            "reason": "timeout",
            "seed": None,
            "output_folder": None,
            "temp_dir": str(temp_work_dir),
            "stdout": exc.stdout or "",
            "stderr": exc.stderr or ""
        }
    except Exception as exc:
        return {
            "success": False,
            "reason": f"exception: {exc}",
            "seed": None,
            "output_folder": None,
            "temp_dir": str(temp_work_dir),
            "stdout": "",
            "stderr": ""
        }

    stdout = result.stdout or ""
    stderr = result.stderr or ""

    has_success_marker = "no collision" in stdout
    seed_match = re.search(r"seed:\s*(\d+)", stdout)
    seed = seed_match.group(1) if seed_match else None

    output_folder_path = None
    if has_success_marker and seed:
        expected_folder = worker_output_base.parent / f"{worker_output_base.name}_seed{seed}"
        if expected_folder.exists():
            output_folder_path = expected_folder
        else:
            has_success_marker = False

    success = has_success_marker and output_folder_path is not None
    reason = None
    if not success:
        if not has_success_marker:
            reason = "missing_success_marker"
        elif not seed:
            reason = "missing_seed"
        else:
            reason = "missing_output_dir"

    return {
        "success": success,
        "reason": reason,
        "seed": seed,
        "output_folder": str(output_folder_path) if output_folder_path else None,
        "temp_dir": str(temp_work_dir),
        "stdout": stdout,
        "stderr": stderr
    }


def move_output_to_final(output_folder_path, final_output_dir):
    """将生成的文件夹移动到最终输出目录。"""

    if not output_folder_path:
        return False

    try:
        src = Path(output_folder_path)
        dest_root = Path(final_output_dir)
        dest_root.mkdir(parents=True, exist_ok=True)

        dest = dest_root / src.name
        if dest.exists():
            timestamp = int(time.time() * 1_000_000)
            dest = dest_root / f"{src.name}_{timestamp}"

        shutil.move(str(src), str(dest))
        return True
    except Exception as exc:
        print(f"Error moving output folder: {exc}")
        return False


def cleanup_temp_dir(temp_dir):
    """清理临时目录。"""

    if not temp_dir:
        return

    try:
        tmp_path = Path(temp_dir)
        if tmp_path.exists():
            shutil.rmtree(tmp_path)
    except Exception as exc:
        print(f"Warning: Failed to cleanup temp dir {temp_dir}: {exc}")


def main():
    """主函数：并行调度数据生成任务"""
    
    # 检查可执行文件是否存在
    if not os.path.exists(EXE_PATH):
        print(f"Error: Executable not found at {EXE_PATH}")
        print("Please build the project first or update EXE_PATH in the script")
        sys.exit(1)
    
    # 设置工作进程数
    worker_count = WORKER_COUNT
    if worker_count is None:
        cpu_count = os.cpu_count() or 4
        worker_count = max(1, cpu_count - 2)
    
    print("=" * 60)
    print("并行数据生成脚本")
    print("=" * 60)
    print(f"可执行文件: {EXE_PATH}")
    print(f"输出目录名: {OUTPUT_DIR_NAME}")
    print(f"任务类型: {TASK_TYPE if TASK_TYPE else '默认'}")
    print(f"目标数量: {TARGET_COUNT}")
    print(f"并行进程数: {worker_count}")
    print(f"最终输出目录: {FINAL_OUTPUT_DIR}")
    print("=" * 60)
    
    # 创建最终输出目录
    os.makedirs(FINAL_OUTPUT_DIR, exist_ok=True)
    
    # 初始化统计
    success_count = 0
    total_attempts = 0
    start_time = time.time()
    
    executor = ProcessPoolExecutor(max_workers=worker_count)

    futures = {}
    task_id = 0

    def submit_task():
        nonlocal task_id
        future = executor.submit(
            run_generator_task,
            task_id,
            EXE_PATH,
            OUTPUT_DIR_NAME,
            TASK_TYPE
        )
        futures[future] = task_id
        task_id += 1

    for _ in range(worker_count):
        submit_task()

    target_met = False

    try:
        while futures:
            for future in as_completed(list(futures.keys())):
                task_num = futures.pop(future)
                total_attempts += 1

                try:
                    result = future.result()
                except Exception as exc:
                    print(f"[Task {task_num}] raised exception: {exc}")
                    continue

                temp_dir = result.get("temp_dir") if isinstance(result, dict) else None

                if isinstance(result, dict) and result.get("success"):
                    seed = result.get("seed", "?")
                    output_folder = result.get("output_folder")
                    if move_output_to_final(output_folder, FINAL_OUTPUT_DIR):
                        success_count += 1
                        print(f"[进度 {success_count}/{TARGET_COUNT}] 成功生成数据 (seed: {seed}, 总尝试: {total_attempts})")
                    else:
                        fallback = output_folder or "<unknown>"
                        print(f"[Task {task_num}] 已生成 seed {seed} 数据但移动失败，结果仍位于: {fallback}")
                        print(f"[进度 {success_count}/{TARGET_COUNT}], 总尝试: {total_attempts})")
                else:
                    reason = result.get("reason") if isinstance(result, dict) else "unknown"
                    stdout_snippet = (result.get("stdout") or "")[:200] if isinstance(result, dict) else ""
                    stderr_snippet = (result.get("stderr") or "")[:200] if isinstance(result, dict) else ""
                    print(f"[Task {task_num}] 失败，原因: {reason}")
                    if stdout_snippet:
                        print(f"  stdout: {stdout_snippet.strip()}")
                    if stderr_snippet:
                        print(f"  stderr: {stderr_snippet.strip()}")

                cleanup_temp_dir(temp_dir)

                if success_count >= TARGET_COUNT:
                    target_met = True
                    print(f"\n已达到目标数量 {TARGET_COUNT}，停止任务提交...")
                    break

                submit_task()

            if target_met:
                break

    except KeyboardInterrupt:
        print("\n\n用户中断，正在停止...")
        target_met = True
    finally:
        executor.shutdown(wait=not target_met, cancel_futures=target_met)
    
    # 打印最终统计
    elapsed_time = time.time() - start_time
    success_rate = (success_count / total_attempts * 100) if total_attempts else 0
    avg_time = (elapsed_time / success_count) if success_count else 0
    print("\n" + "=" * 60)
    print("生成完成!")
    print("=" * 60)
    print(f"成功生成: {success_count} 组")
    print(f"总尝试次数: {total_attempts}")
    print(f"成功率: {success_rate:.2f}%")
    print(f"总耗时: {elapsed_time:.2f} 秒")
    print(f"平均每组耗时: {avg_time:.2f} 秒")
    print(f"输出目录: {FINAL_OUTPUT_DIR}")
    print("=" * 60)


if __name__ == "__main__":
    main()
