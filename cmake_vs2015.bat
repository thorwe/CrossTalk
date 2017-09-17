mkdir build32 & pushd build32
cmake -G "Visual Studio 15 2017" -DCMAKE_PREFIX_PATH="D:\\WinHdd\\Qt\\5.6\\msvc2015" ..
popd
mkdir build64 & pushd build64
cmake -G "Visual Studio 15 2017 Win64"  -DCMAKE_PREFIX_PATH="D:\\WinHdd\\Qt\\5.6\\msvc2015_64" ..
popd
REM cmake --build build32 --config Release
REM cmake --build build64 --config Release
REM cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_PREFIX_PATH="D:\\WinHdd\\Qt\\5.6\\msvc2015_64"