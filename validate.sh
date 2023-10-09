cmake -S . -B cmake-build && cd cmake-build && cmake --build . -j4 && cd unittest && ctest
echo "please check cmake CTest result. Press any key to continue."
read
xmake && xmake run unittest