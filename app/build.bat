emcmake cmake . -B build_emscripten -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build_emscripten

python app\imgui_bundle\external\hello_imgui\hello_imgui\tools\emscripten\webserver_multithread_policy.py -p 8080
cd .\build_emscripten\app\ llvm-dwp -e .\Life-controller-app.wasm -o .\Life-controller-app.wasm.dwp && cd ../../
