cmake . -B build -S . -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=1 -DCMAKE_CXX_COMPILER=clang++
cmake --build build --target=kappa++ -j 8
