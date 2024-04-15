#include <string>
int append_file(std::string file_path, std::string file_content);
void set_log_base_dir(std::string dir);

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include <fstream>
#define debug_printf(...) printf(__VA_ARGS__)
void SyncEmscriptenToIndexDB();
#include "include/fast-cpp-csv-parser/csv.h"
class FileSourceBase : public io::ByteSourceBase {
public:
  explicit FileSourceBase(std::string file_path) {
    file = fopen(file_path.c_str(), "r");
    // Tell the std library that we want to do the buffering ourself.
    std::setvbuf(file, 0, _IONBF, 0);
  }

  int read(char *buffer, int size) { return std::fread(buffer, 1, size, file); }

  ~FileSourceBase() { fclose(file);SyncEmscriptenToIndexDB(); }

private:
  FILE *file;
};
#endif

#ifdef _WIN32
#define debug_printf(...) printf(__VA_ARGS__)
#include "include/fast-cpp-csv-parser/csv.h"
class FileSourceBase : public io::ByteSourceBase {
public:
  explicit FileSourceBase(std::string file_path) {
    file = fopen(file_path.c_str(), "r");
    // Tell the std library that we want to do the buffering ourself.
    std::setvbuf(file, 0, _IONBF, 0);
  }

  int read(char *buffer, int size) { return std::fread(buffer, 1, size, file); }

  ~FileSourceBase() { fclose(file); }

private:
  FILE *file;
};
#endif

#ifdef ESP32
#define debug_printf(...) Serial.printf(__VA_ARGS__)
#include "include/fast-cpp-csv-parser/csv.h"
#include <LittleFS.h>
class FileSourceBase : public io::ByteSourceBase {
public:
  explicit FileSourceBase(std::string file_path) {
    file = LittleFS.open(("/" + file_path).c_str(), "r");
  }

  int read(char *buffer, int size) { return file.readBytes(buffer, size); }

  ~FileSourceBase() { file.close(); }

private:
  fs::File file;
};
#endif
