@echo off
color f0
set title=Weight Alarm - Audio File Converter
title %title%
setlocal enabledelayedexpansion

cd /d %~dp0

:: Ask permission to normalize volume
:ask_normalize
if not [%1]==[] (
	set /p do_normalize="Would you like to normalize/louden volume (may take up to 20s per song) [Y/n]? "
	echo.
) else (
	echo To use this script, you must drag and drop one or multiple audio file^(s^) on top of it.
	echo.
	pause
	exit /b
)
:: Check if answer is valid
if "%do_normalize%"=="" set do_normalize=TRUE
if "%do_normalize%"=="y" set do_normalize=TRUE
if "%do_normalize%"=="Y" set do_normalize=TRUE
if "%do_normalize%"=="n" set do_normalize=FALSE
if "%do_normalize%"=="N" set do_normalize=FALSE
if not "%do_normalize%"=="TRUE" if not "%do_normalize%"=="FALSE" (
	set do_normalize=
	goto ask_normalize
)

:: Prepare workspace
call :getpid pid
echo (new ActiveXObject("WScript.Shell")).AppActivate(%pid%); > setfocus.js
if NOT EXIST "Output" (mkdir Output)

:: Iterate through input arguments
for %%x in (%*) do (
	set inFilePath=%%x
	:: Extract file name
	for %%F in (%%x) do set inFileName=%%~nxF

	echo #### Processing "!inFileName!" ####
	
	:: Prompt for the output file name
	:outfileprompt
	set /p outFileName="Output file name (<= 8 chars, w/o extension): "
	call :strlen length outFileName
	if !length! EQU 0 (
		echo ERROR: output file name must not be empty.
		goto outfileprompt
	)
	if !length! GTR 8 (
		echo ERROR: output file name must not exceed 8 characters.
		goto outfileprompt
	)
	if "!outFileName:~-4!"==".wav" (
		echo ERROR: output file name must not end with an extension.
		goto outfileprompt
	)
	if EXIST "Output/!outFileName!.wav" (
		echo ERROR: a file named "Output/!outFileName!.wav" already exists.
		goto outfileprompt
	)
	
	:: Convert audio file to PCM unsigned 8-bit 32kHz with ffmpeg
	if "%do_normalize%"=="TRUE" (
		start "Convert !inFileName!..." cmd /c "setfocus.js && ffmpeg.exe -hide_banner -i !inFilePath! -af loudnorm=I=-14:LRA=1:TP=-1.5 -ac 1 -ar 32000 -c:a pcm_u8 "Output\!outFileName!.wav""
	) else (
		start "Convert !inFileName!..." cmd /c "setfocus.js && ffmpeg.exe -hide_banner -i !inFilePath! -ac 1 -ar 32000 -c:a pcm_u8 "Output\!outFileName!.wav""
	)
	
	set outFileName=
	echo.
)

:end
echo.
echo Your audio files will be converted and exported to the "Output" directory once all black windows have closed.
echo.
pause
del setfocus.js
exit

REM ******************* strlen **************************
:strlen <resultVar> <stringVar>
(   
    setlocal EnableDelayedExpansion
    (set^ tmp=!%~2!)
    if defined tmp (
        set "len=1"
        for %%P in (4096 2048 1024 512 256 128 64 32 16 8 4 2 1) do (
            if "!tmp:~%%P,1!" NEQ "" ( 
                set /a "len+=%%P"
                set "tmp=!tmp:~%%P!"
            )
        )
    ) ELSE (
        set len=0
    )
)
( 
    endlocal
    set "%~1=%len%"
    exit /b
)


REM ******************* getpid **************************
:getpid <pid>
(   
	setlocal EnableDelayedExpansion
	tasklist /FI "IMAGENAME eq cmd.exe" /v /fo csv | findstr /i "%title%" > pid.txt
	FOR /F "usebackq tokens=2 delims=," %%i in (pid.txt) do (set pid=%%i)
	del pid.txt
)
(
	endlocal
	set "%~1=%pid:"=%"
	exit /b
)