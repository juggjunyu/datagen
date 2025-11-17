@echo off
chcp 936 > nul
setlocal enabledelayedexpansion

:: 可自定义参数
set TARGET_COUNT=50
set BASE_DIR=EFResults

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

echo [%SUCCESS_COUNT%/%TARGET_COUNT%] 尝试生成 #%ATTEMPT_COUNT%, 输出目录: %OUTPUT_DIR%

:: if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: 运行生成器并捕获输出
echo 正在运行generator...
xmake r generator -t 1 -o %OUTPUT_DIR% > "generation_log_EF.txt" 2>&1

:: 显示输出结果
type "generation_log_EF.txt"

:: 检查是否包含"no collision"
findstr /C:"no collision" generation_log_EF.txt > nul
if %ERRORLEVEL% EQU 0 (
    :: 找到"no collision"，增加计数
    set /a SUCCESS_COUNT+=1
    echo 成功! 进度: %SUCCESS_COUNT%/%TARGET_COUNT%
) else (
    echo 未找到"no collision"，失败
    :: 删除没有"no collision"的输出目录
    if exist "%OUTPUT_DIR%" (
        echo 删除目录: %OUTPUT_DIR%
        rd /s /q "%OUTPUT_DIR%" 2>nul
    )
)

:: 删除临时日志文件
del "generation_log_EF.txt"

:: 检查是否达到目标计数
if %SUCCESS_COUNT% GEQ %TARGET_COUNT% (
    echo.
    echo 完成: %TARGET_COUNT% / %ATTEMPT_COUNT% 次尝试
    goto :end
)

:: 短暂暂停以减轻系统负担
timeout /t 1 /nobreak > nul
goto loop

:end
echo 程序结束.

endlocal