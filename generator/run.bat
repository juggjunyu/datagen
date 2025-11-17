@REM @echo off
@REM chcp 936 > nul
@REM setlocal enabledelayedexpansion

@REM :: 可自定义参数
@REM set TARGET_COUNT=10
@REM set BASE_DIR=Results

@REM :: 初始化计数器
@REM set SUCCESS_COUNT=0
@REM set ATTEMPT_COUNT=0

@REM echo 开始生成，目标数量: %TARGET_COUNT%
@REM echo.

@REM :loop
@REM :: 增加尝试计数
@REM set /a ATTEMPT_COUNT+=1

@REM :: 使用一致的目录命名
@REM set OUTPUT_DIR=%BASE_DIR%_%SUCCESS_COUNT%

@REM echo [%SUCCESS_COUNT%/%TARGET_COUNT%] 尝试生成 #%ATTEMPT_COUNT%, 输出目录: %OUTPUT_DIR%

@REM if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

@REM :: 运行生成器并捕获输出
@REM echo 正在运行generator...
@REM xmake r generator -o %OUTPUT_DIR% > "generation_log.txt" 2>&1

@REM :: 显示输出结果
@REM type "generation_log.txt"

@REM :: 检查是否包含"no collision"
@REM findstr /C:"no collision" generation_log.txt > nul
@REM if %ERRORLEVEL% EQU 0 (
@REM     :: 找到"no collision"，增加计数
@REM     set /a SUCCESS_COUNT+=1
@REM     echo 成功! 进度: %SUCCESS_COUNT%/%TARGET_COUNT%
@REM ) else (
@REM     echo 未找到"no collision"，失败
@REM     :: 删除没有"no collision"的输出目录
@REM     if exist "%OUTPUT_DIR%" (
@REM         echo 删除目录: %OUTPUT_DIR%
@REM         rd /s /q "%OUTPUT_DIR%" 2>nul
@REM     )
@REM )

@REM :: 删除临时日志文件
@REM del "generation_log.txt"

@REM :: 检查是否达到目标计数
@REM if %SUCCESS_COUNT% GEQ %TARGET_COUNT% (
@REM     echo.
@REM     echo 完成: %TARGET_COUNT% / %ATTEMPT_COUNT% 次尝试
@REM     goto :end
@REM )

@REM :: 短暂暂停以减轻系统负担
@REM timeout /t 1 /nobreak > nul
@REM goto loop

@REM :end
@REM echo 程序结束.

@REM endlocal