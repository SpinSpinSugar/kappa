cmake . -B build -S . -DENABLE_TESTING=1 -DCMAKE_CXX_COMPILER=clang++
cmake --build build --target=kappa++ -j 8^C
