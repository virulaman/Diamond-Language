@echo off
REM DMO Language Compiler - Windows Build Script
REM Prerequisites: MinGW-w64 or similar GCC compiler for Windows

echo Building DMO Compiler for Windows...

REM Clean previous builds
del /Q *.o dmo.exe 2>NUL

REM Compile all source files
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -g -c main.c -o main.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c lexer.c -o lexer.o  
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c parser.c -o parser.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c ast.c -o ast.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c interpreter.c -o interpreter.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c modules.c -o modules.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c stdlib_funcs.c -o stdlib_funcs.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -c dmo_graphs.c -o dmo_graphs.o
if errorlevel 1 goto error

REM Link executable
echo Linking executable...
gcc -Wall -Wextra -std=c99 -g -o dmo.exe main.o lexer.o parser.o ast.o interpreter.o modules.o stdlib_funcs.o dmo_graphs.o -lm
if errorlevel 1 goto error

echo.
echo ========================================
echo DMO Compiler built successfully!
echo Executable: dmo.exe
echo.
echo Usage: dmo.exe program.dmo
echo ========================================
echo.

REM Test with hello world example
if exist examples\hello.dmo (
    echo Testing with hello.dmo example...
    dmo.exe examples\hello.dmo
)

goto end

:error
echo.
echo ========================================
echo Build failed! Please check errors above.
echo ========================================
pause
exit /b 1

:end
pause