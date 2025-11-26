@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

@REM :: 可自定义参数
set TARGET_COUNT=1000
set BASE_DIR=E:\research\data\Animation\Edge-Edge
@REM Generator will append _seed + random number to output directory name
@REM To create subdirectories under BASE_DIR, output directory should be BASE_DIR\Edge-Edge
@REM Generator will create BASE_DIR\Edge-Edge_seed + random number
set OUTPUT_DIR=%BASE_DIR%\Edge-Edge

@REM :: 初始化计数器
set SUCCESS_COUNT=242
set ATTEMPT_COUNT=0

@REM :: 记录开始时间（使用PowerShell获取精确时间戳，避免整数溢出和跨天问题）
for /f %%t in ('powershell -Command "[int](Get-Date -UFormat %%s)"') do set START_TIMESTAMP=%%t

echo 开始生成，目标数量: %TARGET_COUNT%
echo.

:loop
@REM :: 增加尝试计数
set /a ATTEMPT_COUNT+=1

@REM :: 确保基础目录存在
if not exist "%BASE_DIR%" mkdir "%BASE_DIR%"

echo [!SUCCESS_COUNT!/%TARGET_COUNT%] 尝试生成 #!ATTEMPT_COUNT!, 输出将保存在: %BASE_DIR%

@REM :: 使用唯一的日志文件名避免文件锁定问题
set LOG_FILE=generation_log_EE_!ATTEMPT_COUNT!.txt

@REM :: 运行生成器并捕获输出
echo 正在运行generator...
xmake r generator -t 2 -o "%OUTPUT_DIR%" > "!LOG_FILE!" 2>&1

@REM :: 等待片刻确保文件完全写入
timeout /t 0 /nobreak > nul 2>&1

@REM :: 显示输出结果
type "!LOG_FILE!" 2>nul

@REM :: 提取种子值
set SEED_VALUE=
if exist "!LOG_FILE!" (
    timeout /t 0 /nobreak > nul 2>&1
    for /f "tokens=2" %%s in ('findstr /C:"seed:" "!LOG_FILE!" 2^>nul') do set SEED_VALUE=%%s
)

@REM :: 检查是否包含"no collision"
findstr /C:"no collision" "!LOG_FILE!" > nul 2>&1
if errorlevel 1 (
    echo 未找到"no collision"，失败
    if defined SEED_VALUE (
        set FAILED_DIR=!OUTPUT_DIR!_seed!SEED_VALUE!
        if exist "!FAILED_DIR!" (
            echo 删除目录: !FAILED_DIR!
            rd /s /q "!FAILED_DIR!" 2>nul
        )
    )
) else (
    @REM :: 找到"no collision"，增加计数
    set /a SUCCESS_COUNT+=1
    echo 成功! 进度: !SUCCESS_COUNT!/%TARGET_COUNT%
)

@REM :: 删除临时日志文件（使用延迟删除避免锁定）
if exist "!LOG_FILE!" (
    timeout /t 0 /nobreak > nul 2>&1
    del "!LOG_FILE!" > nul 2>&1
)

@REM :: 检查是否达到目标计数
if !SUCCESS_COUNT! GEQ %TARGET_COUNT% (
    echo.
    echo 完成: %TARGET_COUNT% / !ATTEMPT_COUNT! 次尝试
    goto :end
)

@REM :: 短暂暂停以减轻系统负担
timeout /t 1 /nobreak > nul
goto loop

:end
@REM :: 记录结束时间并计算总运行时间
for /f %%t in ('powershell -Command "[int](Get-Date -UFormat %%s)"') do set END_TIMESTAMP=%%t

@REM :: 计算总时间（秒）
set /a TOTAL_SECONDS=%END_TIMESTAMP% - %START_TIMESTAMP%

@REM :: 转换为可读格式
set /a ELAPSED_HOURS=%TOTAL_SECONDS% / 3600
set /a ELAPSED_MINUTES=(%TOTAL_SECONDS% - %ELAPSED_HOURS% * 3600) / 60
set /a ELAPSED_SECONDS=%TOTAL_SECONDS% - %ELAPSED_HOURS% * 3600 - %ELAPSED_MINUTES% * 60

echo 程序结束.
echo 总运行时间: %ELAPSED_HOURS%小时 %ELAPSED_MINUTES%分钟 %ELAPSED_SECONDS%秒 (%TOTAL_SECONDS%秒总计)

endlocal