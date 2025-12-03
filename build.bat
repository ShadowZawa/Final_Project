@echo off
echo Start building...
g++ -g main.cpp -o main.exe -I. -I.\external\ftxui\include -L.\external\ftxui\lib -lftxui-component -lftxui-dom -lftxui-screen
if %errorlevel% equ 0 (
    echo Build succeeded!
    echo Executable: main.exe
) else (
    echo Build failed!
)
pause