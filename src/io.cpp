#include "io.hpp"
#include <stdio.h>


static std::string log_base_dir = "";

void set_log_base_dir(std::string dir) { log_base_dir = dir; }

#ifdef EMSCRIPTEN
#include <fstream>

// Sync Emscripten => IndexedDB
void SyncEmscriptenToIndexDB()
{
    std::string script = R"(
        FS.syncfs(false, function (err) {
            console.log("SyncEmscriptenToIndexDB ended: err= " + err);
        });
    )";
    emscripten_run_script(script.c_str());
}

int append_file(std::string file_path, std::string file_content)
{
    std::ofstream outfile;
    outfile.open(log_base_dir + "/" + file_path, std::ios_base::app);
    printf("write to %s\n", (log_base_dir + "/" + file_path).c_str());
    outfile << file_content;
    SyncEmscriptenToIndexDB();
    return 0;
}
#endif

#ifdef _WIN32
#include <fstream>
int append_file(std::string file_path, std::string file_content)
{
    std::ofstream outfile;
    outfile.open(log_base_dir + "/" + file_path, std::ios_base::app);
    outfile << file_content;
    return 0;
}
#endif

#ifdef ESP32
#include <LittleFS.h>

int append_file(std::string file_path, std::string file_content)
{
    File file = LittleFS.open((log_base_dir + "/" + file_path).c_str(), FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return 1;
    }
    if (!file.print(file_content.c_str()))
    {
        Serial.println("- append failed");
        file.close();
        return 1;
    }
    file.close();
    return 0;
}

#endif
