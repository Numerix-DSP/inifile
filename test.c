// Copyright (c) 2026 Delta Numerix, All rights reserved.

#include "inifile.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    ini_file ini = {0};
    char value[1024];
    const char *path = argc > 1 ? argv[1] : "testfile.ini";
    if (!ini_init(&ini, path)) return 1;
    if (!ini_read(&ini, "SIZE", value, sizeof value)) {
        fprintf(stderr, "Could not read SIZE\n");
        ini_destroy(&ini);
        return 1;
    }
    printf("SIZE: %s\n", value);
    if (!ini_write(&ini, "test", "Updated from C") ||
        !ini_read(&ini, "test", value, sizeof value) ||
        strcmp(value, "Updated from C") != 0) {
        fprintf(stderr, "Write/read verification failed\n");
        ini_destroy(&ini);
        return 1;
    }
    puts("C read/write test passed");
    ini_destroy(&ini);
    return 0;
}
