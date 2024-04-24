#include <cstdio>
#include <string>
#include <cassert>
int append_file(const std::string &file_path, const std::string &file_content);
void set_log_base_dir(std::string dir);

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include <fstream>
#include <cstring>
#define debug_printf(...) printf(__VA_ARGS__)
void SyncEmscriptenToIndexDB();
#include "include/fast-cpp-csv-parser/csv.h"
class FileSourceBase : public io::ByteSourceBase
{
   public:
    explicit FileSourceBase(std::string file_path)
    {
        file = fopen(file_path.c_str(), "r");
        if (file == NULL)
        {
            debug_printf("file open failed. Try to create new one.\n");
            file = fopen(file_path.c_str(), "w");
            if (file == NULL)
            {
                debug_printf("can't create file.\n");
            } else {
                // fprintf(file, "\n");
                fclose(file);
                file = fopen(file_path.c_str(), "r");
                assert(file);
            }
        }
        // Tell the std library that we want to do the buffering ourself.
        std::setvbuf(file, 0, _IONBF, 0);
    }

    int read(char *buffer, int size) { return std::fread(buffer, 1, size, file); }

    ~FileSourceBase()
    {
        fclose(file);
        SyncEmscriptenToIndexDB();
    }

   private:
    FILE *file;
};

class EmptySourceBase : public io::ByteSourceBase
{
   public:
        int read(char *buffer, int size) {
            if (size < sizeof(buffer))
            {
                memcpy(buffer, temp, size);
                return size;
            } else {
                memcpy(buffer, temp, sizeof(temp));
                return sizeof(temp);
            }
        }

   private:
    char temp[256] = "CREATE  , Tue Apr 23 01:41:17 2024, 00:00:00, test, test event, 1713807677, 1713807677\n";
};

class CSVReader
{
   public:
    io::CSVReader<7> csv_reader;
    CSVReader(std::string file_path)
        : csv_reader(file_path, std::unique_ptr<io::ByteSourceBase>(new FileSourceBase(file_path)))
    {
    }
    CSVReader() : csv_reader("empty", std::unique_ptr<io::ByteSourceBase>(new EmptySourceBase()))
    {
    }
    ~CSVReader() {}
};
#endif

#ifdef _WIN32
#define debug_printf(...) printf(__VA_ARGS__)
#include "include/fast-cpp-csv-parser/csv.h"
#include <cstring>
class FileSourceBase : public io::ByteSourceBase
{
   public:
    explicit FileSourceBase(std::string file_path)
    {
        file = fopen(file_path.c_str(), "r");
        if (file == NULL)
        {
            debug_printf("file open failed. Try to create new one.\n");
            file = fopen(file_path.c_str(), "w");
            if (file == NULL)
            {
                debug_printf("can't create file.\n");
            } else {
                // fprintf(file, "\n");
                fclose(file);
                file = fopen(file_path.c_str(), "r");
                assert(file);
            }
        }
        // Tell the std library that we want to do the buffering ourself.
        std::setvbuf(file, 0, _IONBF, 0);
    }

    int read(char *buffer, int size) { return std::fread(buffer, 1, size, file); }

    ~FileSourceBase() { fclose(file); }

   private:
    FILE *file;
};

class EmptySourceBase : public io::ByteSourceBase
{
   public:
        int read(char *buffer, int size) {
            if (size < sizeof(buffer))
            {
                memcpy(buffer, temp, size);
                return size;
            } else {
                memcpy(buffer, temp, sizeof(temp));
                return sizeof(temp);
            }
        }

   private:
    char temp[256] = "CREATE  , Tue Apr 23 01:41:17 2024, 00:00:00, test, test event, 1713807677, 1713807677\n";
};

class CSVReader
{
   public:
    io::CSVReader<7> csv_reader;
    CSVReader(std::string file_path)
        : csv_reader(file_path, std::unique_ptr<io::ByteSourceBase>(new FileSourceBase(file_path)))
    {
    }
    CSVReader() : csv_reader("empty", std::unique_ptr<io::ByteSourceBase>(new EmptySourceBase()))
    {
    }
    ~CSVReader() {}
};
#endif

#ifdef ESP32
#define debug_printf(...) Serial.printf(__VA_ARGS__)
#include "include/fast-cpp-csv-parser/csv.h"
#include <LittleFS.h>
class FileSourceBase : public io::ByteSourceBase
{
   public:
    explicit FileSourceBase(std::string file_path) { file = LittleFS.open(("/" + file_path).c_str(), "r"); }

    int read(char *buffer, int size) { return file.readBytes(buffer, size); }

    ~FileSourceBase() { file.close(); }

   private:
    fs::File file;
};
class CSVReader
{
   public:
    io::CSVReader<7> csv_reader;
    CSVReader(std::string file_path)
        : csv_reader(file_path, std::unique_ptr<io::ByteSourceBase>(new FileSourceBase(file_path)))
    {
    }
    ~CSVReader() {}
};
#endif
