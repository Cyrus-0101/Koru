@ECHO OFF
REM Build Everything — runs engine and testbed builds in sequence

ECHO "Building everything..."

REM Go into engine folder and run build.bat
PUSHD engine
CALL build.bat
POPD

REM Check if last command failed
IF %ERRORLEVEL% NEQ 0 (
    echo Error:%ERRORLEVEL% && exit
)

REM Go into testbed folder and run build.bat
PUSHD testbed
CALL build.bat
POPD

REM Check again for errors
IF %ERRORLEVEL% NEQ 0 (
    echo Error:%ERRORLEVEL% && exit
)

REM Success message
ECHO "All assemblies built successfully."