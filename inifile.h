// Copyright (c) 2026 Delta Numerix, All rights reserved.

#ifndef INIFILE_H
#define INIFILE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ini_file {
    char *filename;
} ini_file;

/* Returns 1 on success, 0 on failure. */
int ini_init(ini_file *ini, const char *filename);
void ini_destroy(ini_file *ini);
/* Reads into caller-provided buffer; fails if value does not fit. */
int ini_read(const ini_file *ini, const char *key, char *value, size_t capacity);
int ini_write(const ini_file *ini, const char *key, const char *value);

#ifdef __cplusplus
}

#include <string>
#include <vector>

/* Convenience C++ interface; the implementation remains pure C. */
class IniFile {
    ini_file file_{};
public:
    explicit IniFile(const std::string& filename) { ini_init(&file_, filename.c_str()); }
    ~IniFile() { ini_destroy(&file_); }
    IniFile(const IniFile&) = delete;
    IniFile& operator=(const IniFile&) = delete;
    bool ReadIniStringFromFile(const std::string& key, std::string *value) const {
        if (!value) return false;
        size_t capacity = 256;
        while (capacity <= 16 * 1024 * 1024) {
            std::vector<char> buffer(capacity);
            if (ini_read(&file_, key.c_str(), buffer.data(), buffer.size())) {
                *value = buffer.data();
                return true;
            }
            /* A missing key and an oversized value both return failure. */
            capacity *= 2;
        }
        return false;
    }
    bool WriteIniStringToFile(const std::string& key, const std::string& value) const {
        return ini_write(&file_, key.c_str(), value.c_str()) != 0;
    }
};
#endif
#endif
