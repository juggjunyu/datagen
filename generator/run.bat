@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

:: 可自定义参数
set TARGET_COUNT=1000
set BASE_DIR=E:\research\data\Animation\Face-Face
:: 生成器会在输出目录名后添加 _seed{seed}
:: 为了在 BASE_DIR 下创建子目录，输出目录需要是 BASE_DIR\Face-Face
:: 这样生成器会创建 BASE_DIR\Face-Face_seed{seed}
set OUTPUT_DIR=%BASE_DIR%\Face-Face

:: 初始化计数器
set SUCCESS_COUNT=600
set ATTEMPT_COUNT=0

:: 记录开始时间
for /f "tokens=1-4 delims=:." %%A in ("%time%") do set START_TIME=%%A%%B%%C%%D

echo 开始生成，目标数量: %TARGET_COUNT%
echo.

:loop
:: 增加尝试计数
set /a ATTEMPT_COUNT+=1

:: 确保基础目录存在
if not exist "%BASE_DIR%" mkdir "%BASE_DIR%"

echo [%SUCCESS_COUNT%/%TARGET_COUNT%] try generate #%ATTEMPT_COUNT%, output will be in: %BASE_DIR%

:: 运行生成器并捕获输出
echo Running generator...
xmake r generator -o "%OUTPUT_DIR%" > "generation_log.txt" 2>&1

:: 显示输出结果
type "generation_log.txt"

:: 提取 seed 值
set SEED_VALUE=
for /f "tokens=2" %%s in ('findstr /C:"seed:" generation_log.txt') do set SEED_VALUE=%%s

:: 检查是否包含"no collision"
findstr /C:"no collision" generation_log.txt > nul
if %ERRORLEVEL% EQU 0 (
    :: Found "no collision", increment count
    set /a SUCCESS_COUNT+=1
    echo Success! Progress: %SUCCESS_COUNT%/%TARGET_COUNT%
    :: 生成器会自动在 BASE_DIR 下创建 Face-Face_seed{seed} 目录，无需额外操作
) else (
    echo "no collision" not found, failure
    :: 删除生成的目录
    if defined SEED_VALUE (
        set FAILED_DIR=%OUTPUT_DIR%_seed!SEED_VALUE!
        if exist "!FAILED_DIR!" (
            echo Deleting directory: !FAILED_DIR!
            rd /s /q "!FAILED_DIR!" 2>nul
        )
    )
)

:: 删除临时日志文件
del "generation_log.txt"

:: 检查是否达到目标计数
if %SUCCESS_COUNT% GEQ %TARGET_COUNT% (
    echo.
    echo Completed: %TARGET_COUNT% / %ATTEMPT_COUNT% attempts
    goto :end
)

:: Short pause to reduce system load
timeout /t 1 /nobreak > nul
goto loop

:end
:: 记录结束时间
for /f "tokens=1-4 delims=:." %%A in ("%time%") do set END_TIME=%%A%%B%%C%%D

:: 计算总计时
set /a ELAPSED_TIME_MS=%END_TIME% - %START_TIME%
set /a ELAPSED_TIME_SEC=%ELAPSED_TIME_MS% / 1000
echo Total time: %ELAPSED_TIME_SEC% seconds