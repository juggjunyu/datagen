import os
import subprocess
import tempfile
import sys

# ========================================================
# 🔴 请修改这里为你真实的 exe 路径 (使用绝对路径或相对路径均可)
EXE_PATH = r"E:\\research\\datagen\\build\\windows\\x64\\debug\\generator1.exe" 
# ========================================================

def diagnose():
    # 1. 转为绝对路径，排除路径写错的干扰
    abs_path = os.path.abspath(EXE_PATH)
    print(f"📍 目标 EXE: {abs_path}")

    if not os.path.exists(abs_path):
        print("❌ 错误: 找不到该文件！请检查路径拼写。")
        return

    # 2. 模拟真实环境（去临时目录运行）
    print("🔍 正在模拟运行环境...")
    
    with tempfile.TemporaryDirectory() as temp_dir:
        print(f"📂 临时目录: {temp_dir}")
        
        # 准备环境：把 exe 所在的目录加到 PATH，防止找不到 DLL
        exe_dir = os.path.dirname(abs_path)
        env = os.environ.copy()
        env["PATH"] = exe_dir + os.pathsep + env.get("PATH", "")
        
        # 构造命令
        cmd = [abs_path, "-o", "debug_output"]
        
        print(f"🚀 执行命令: {cmd}")
        print("-" * 40)
        
        try:
            # 关键：这里不要用 capture_output=True，我们要让它直接输出到终端！
            # 这样如果缺少 DLL，Windows 的弹窗或者报错能直接看到
            result = subprocess.run(
                cmd,
                cwd=temp_dir,   # 切换到临时目录
                env=env,        # 注入环境变量
                timeout=10      # 设置超时
            )
            
            print("-" * 40)
            print(f"📊 退出代码 (Return Code): {result.returncode}")
            
            if result.returncode == 0:
                print("✅ 运行成功！(问题可能出在原来的脚本逻辑上)")
            else:
                analyze_error(result.returncode)

        except subprocess.TimeoutExpired:
            print("❌ 超时：程序卡住了。")
        except Exception as e:
            print(f"❌ Python 调用出错: {e}")

def analyze_error(code):
    print("\n💀 错误分析:")
    if code == 3221225781 or code == -1073741515: # 0xC0000135
        print("🔴 【缺少 DLL】 (STATUS_DLL_NOT_FOUND)")
        print("   你的 exe 依赖某些 .dll 文件，但在运行时找不到。")
        print("   解决办法：把 build 目录下的所有 .dll 复制到和 exe 同级，或者检查 PATH。")
    elif code == 3221225477 or code == -1073741819: # 0xC0000005
        print("🔴 【内存访问冲突】 (Access Violation)")
        print("   C++ 代码里有空指针或数组越界。")
    else:
        print(f"🔴 未知错误码 {code}。通常意味着程序刚启动就崩了。")

if __name__ == "__main__":
    diagnose()