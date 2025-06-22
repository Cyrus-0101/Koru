@ECHO OFF
REM Build Everything — runs engine and testbed builds in sequence

ECHO "Building everything..."

REM Go into engine folder and run build.bat
@REM PUSHD engine
@REM CALL build.bat
@REM POPD
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)
@REM PUSHD testbed
@REM CALL build.bat
@REM POPD
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Engine
make -f "Makefile.engine.windows.mak" all
IF %ERRORLEVEL% NEQ 0 (
    echo Error:%ERRORLEVEL% && exit
)

REM Go into testbed folder and run build.bat
make -f "Makefile.testbed.windows.mak" all

REM Check again for errors
IF %ERRORLEVEL% NEQ 0 (
    echo Error:%ERRORLEVEL% && exit
)

REM Success message
ECHO "All assemblies built successfully."