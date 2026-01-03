@echo off
REM 切换到脚本所在目录
cd /d "%~dp0"

echo ota packer...
"PackagerTools.exe" --src ..\..\app.bin --rbl ..\..\ota_app.rbl --part app --ver v1.0.0 --prod LE001 --qlz

if %errorlevel% neq 0 (
    echo ota pack failed!
    pause
    exit /b 1
)
python3 addff.py ..\..\ota_app.rbl
echo ota pack success!
