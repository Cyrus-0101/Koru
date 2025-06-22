#!/bin/bash
# Build script for rebuilding everything on Linux

# Enable command echoing
set -x

# Start message
echo "Building everything..."

# Enter engine directory and source its build script
# pushd engine
# source build.sh
# popd
make -f Makefile.engine.linux.mak all

# Check for failure
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error:"$ERRORLEVEL && exit
fi

# Enter testbed directory and source its build script
# pushd testbed
# source build.sh
# popd
make -f Makefile.testbed.linux.mak all

# Check again
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error:"$ERRORLEVEL && exit
fi

# Final success message
echo "All assemblies built successfully."