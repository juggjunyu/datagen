@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

REM Customizable parameters
set TARGET_COUNT=1000
set BASE_DIR=E:\research\data\Animation\Edge-Face
REM Generator will append _seed + random number to output directory name
REM To create subdirectories under BASE_DIR, output directory should be BASE_DIR\Edge-Face
REM Generator will create BASE_DIR\Edge-Face_seed + random number
set OUTPUT_DIR=%BASE_DIR%\Edge-Face

REM Initialize counters
set SUCCESS_COUNT=240
set ATTEMPT_COUNT=0

REM Record start time using PowerShell for precise timestamp
for /f %%t in ('powershell -Command "[int](Get-Date -UFormat %%s)"') do set START_TIMESTAMP=%%t

echo Starting generation, target count: %TARGET_COUNT%
echo.

:loop
REM Increment attempt count
set /a ATTEMPT_COUNT+=1

REM Ensure base directory exists
if not exist "%BASE_DIR%" mkdir "%BASE_DIR%"

echo [!SUCCESS_COUNT!/%TARGET_COUNT%] Attempt #!ATTEMPT_COUNT!, output will be saved in: %BASE_DIR%

REM Use unique log filename for each attempt to avoid file locking issues
set LOG_FILE=generation_log_EF_!ATTEMPT_COUNT!.txt

REM Run generator and capture output
echo Running generator...
xmake r generator -t 1 -o "%OUTPUT_DIR%" > "!LOG_FILE!" 2>&1

REM Wait a moment to ensure file is fully written
timeout /t 0 /nobreak > nul 2>&1

REM Display output
type "!LOG_FILE!" 2>nul

REM Extract seed value
set SEED_VALUE=
if exist "!LOG_FILE!" (
    timeout /t 0 /nobreak > nul 2>&1
    for /f "tokens=2" %%s in ('findstr /C:"seed:" "!LOG_FILE!" 2^>nul') do set SEED_VALUE=%%s
)

REM Check if contains "no collision"
findstr /C:"no collision" "!LOG_FILE!" > nul 2>&1
if errorlevel 1 (
    echo "no collision" not found, failed
    REM Delete generated directory
    if defined SEED_VALUE (
        set FAILED_DIR=!OUTPUT_DIR!_seed!SEED_VALUE!
        if exist "!FAILED_DIR!" (
            echo Deleting directory: !FAILED_DIR!
            rd /s /q "!FAILED_DIR!" 2>nul
        )
    )
) else (
    REM Found "no collision", increment count
    set /a SUCCESS_COUNT+=1
    echo Success! Progress: !SUCCESS_COUNT!/%TARGET_COUNT%
)

REM Delete temporary log file (use delayed deletion to avoid locking)
if exist "!LOG_FILE!" (
    timeout /t 0 /nobreak > nul 2>&1
    del "!LOG_FILE!" > nul 2>&1
)

REM Check if target count reached
if !SUCCESS_COUNT! GEQ %TARGET_COUNT% (
    echo.
    echo Completed: %TARGET_COUNT% / !ATTEMPT_COUNT! attempts
    goto :cleanup
)

REM Brief pause to reduce system load
timeout /t 1 /nobreak > nul
goto loop

:cleanup
REM Clean up any remaining log files
for %%f in (generation_log_EF_*.txt) do (
    if exist "%%f" del "%%f" > nul 2>&1
)
goto :end

:end
REM Record end time using PowerShell for precise timestamp
for /f %%t in ('powershell -Command "[int](Get-Date -UFormat %%s)"') do set END_TIMESTAMP=%%t

REM Calculate total time in seconds
set /a ELAPSED_SECONDS=%END_TIMESTAMP% - %START_TIMESTAMP%

REM Convert to human readable format
set /a ELAPSED_HOURS=%ELAPSED_SECONDS% / 3600
set /a ELAPSED_MINUTES=(%ELAPSED_SECONDS% - %ELAPSED_HOURS% * 3600) / 60
set /a ELAPSED_REMAINING_SECONDS=%ELAPSED_SECONDS% - %ELAPSED_HOURS% * 3600 - %ELAPSED_MINUTES% * 60

echo Total time: %ELAPSED_HOURS% hours %ELAPSED_MINUTES% minutes %ELAPSED_REMAINING_SECONDS% seconds (%ELAPSED_SECONDS% seconds total)
echo Program ended.

REM Final cleanup
for %%f in (generation_log_EF_*.txt) do (
    if exist "%%f" del "%%f" > nul 2>&1
)

endlocal
