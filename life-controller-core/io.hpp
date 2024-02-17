#include <string>
int append_file(std::string file_path, std::string file_content);
FILE* get_log_file_handle(const char* filename);

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
#endif
