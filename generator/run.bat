@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

:: 可自定义参数
set TARGET_COUNT=600
set BASE_DIR=E:\research\data\Animation\Face-Face

:: 初始化计数器
set SUCCESS_COUNT=0
set ATTEMPT_COUNT=0

echo 开始生成，目标数量: %TARGET_COUNT%
echo.

:loop
:: 增加尝试计数
set /a ATTEMPT_COUNT+=1

:: 使用一致的目录命名
set OUTPUT_DIR=%BASE_DIR%_%SUCCESS_COUNT%

echo [%SUCCESS_COUNT%/%TARGET_COUNT%] try generate #%ATTEMPT_COUNT%, output directory: %OUTPUT_DIR%

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: 运行生成器并捕获输出
echo Running generator...
xmake r generator -o %OUTPUT_DIR% > "generation_log.txt" 2>&1

:: 显示输出结果
type "generation_log.txt"

:: 检查是否包含"no collision"
findstr /C:"no collision" generation_log.txt > nul
if %ERRORLEVEL% EQU 0 (
    :: Found "no collision", increment count
    set /a SUCCESS_COUNT+=1
    echo Success! Progress: %SUCCESS_COUNT%/%TARGET_COUNT%
) else (
    echo "no collision" not found, failure
    :: Delete output directory without "no collision"
    if exist "%OUTPUT_DIR%" (
        echo Deleting directory: %OUTPUT_DIR%
        rd /s /q "%OUTPUT_DIR%" 2>nul
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
echo Program ended.

endlocal