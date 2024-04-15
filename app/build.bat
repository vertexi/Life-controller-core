emcmake cmake . -B build_emscripten -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build_emscripten

python app\imgui_bundle\external\hello_imgui\hello_imgui\tools\emscripten\webserver_multithread_policy.py -p 8080
