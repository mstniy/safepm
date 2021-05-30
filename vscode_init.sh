# The vscode extension clangd requires compile_commands.json to work properly

if ! command -v bear &> /dev/null
then
    echo "Make sure bear is installed." 2>&1
    exit 1
fi

cd "$(dirname "$0")"
# Clean all build artifacts to force a complete rebuild
cd pmdk
make clobber
cd ..
rm -rf build
mkdir build
cd build
cmake ..
# Use bear to watch the build process.
# cmake's CMAKE_EXPORT_COMPILE_COMMANDS is not enough, because pmdk is not a cmake project.
bear make
mv compile_commands.json ..
