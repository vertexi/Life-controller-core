#include <stdio.h>
#include "io.hpp"

#ifdef _WIN32
#include <fstream>
int append_file(std::string file_path, std::string file_content) {
    std::ofstream outfile;
    outfile.open(file_path, std::ios_base::app);
    outfile << file_content;
    return 0;
}
#endif

#ifdef ESP32
#include <LittleFS.h>

int append_file(std::string file_path, std::string file_content){
    File file = LittleFS.open(("/" + file_path).c_str(), FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return 1;
    }
    if(!file.print(file_content.c_str())){
        Serial.println("- append failed");
        file.close();
        return 1;
    }
    file.close();
    return 0;
}


#endif
