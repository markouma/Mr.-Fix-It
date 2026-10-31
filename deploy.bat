@echo off
set PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%

gcc -Wall -static repair_web.c -o repair.exe
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

copy /Y repair.exe C:\xampp\cgi-bin\repair.exe
if not exist C:\xampp\htdocs\repair mkdir C:\xampp\htdocs\repair
copy /Y style.css C:\xampp\htdocs\repair\style.css
copy /Y index.html C:\xampp\htdocs\repair\index.html

echo.
echo Done. Open http://localhost:8081/repair/ in your browser.
start http://localhost:8081/repair/
pause
