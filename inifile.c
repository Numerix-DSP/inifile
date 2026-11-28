#include "inifile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *duplicate(const char *s) {
    size_t n;
    char *p;
    if (!s) return NULL;
    n = strlen(s) + 1;
    p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static int valid_key(const char *key) {
    return key && *key && !strpbrk(key, "\r\n[]");
}

static int valid_value(const char *value) {
    return value && !strpbrk(value, "\r\n");
}

/* Read an arbitrarily long line, stripping its line ending. */
static char *read_line(FILE *fp) {
    size_t length = 0, capacity = 128;
    int ch;
    char *line = (char *)malloc(capacity);
    if (!line) return NULL;
    while ((ch = fgetc(fp)) != EOF && ch != '\n') {
        if (length + 1 >= capacity) {
            char *next;
            if (capacity > ((size_t)-1) / 2) { free(line); return NULL; }
            capacity *= 2;
            next = (char *)realloc(line, capacity);
            if (!next) { free(line); return NULL; }
            line = next;
        }
        line[length++] = (char)ch;
    }
    if (ch == EOF && length == 0) { free(line); return NULL; }
    if (length && line[length - 1] == '\r') --length;
    line[length] = '\0';
    return line;
}

static int is_key(const char *line, const char *key) {
    size_t n = strlen(key);
    return line[0] == '[' && strlen(line) == n + 2 &&
           memcmp(line + 1, key, n) == 0 && line[n + 1] == ']';
}

int ini_init(ini_file *ini, const char *filename) {
    if (!ini || !filename || !*filename) return 0;
    ini->filename = duplicate(filename);
    return ini->filename != NULL;
}

void ini_destroy(ini_file *ini) {
    if (ini) { free(ini->filename); ini->filename = NULL; }
}

int ini_read(const ini_file *ini, const char *key, char *value, size_t capacity) {
    FILE *fp;
    char *line;
    int found = 0;
    if (!ini || !ini->filename || !valid_key(key) || !value || !capacity) return 0;
    fp = fopen(ini->filename, "rb");
    if (!fp) return 0;
    while ((line = read_line(fp)) != NULL) {
        if (is_key(line, key)) {
            char *next = read_line(fp);
            if (next) {
                size_t n = strlen(next);
                if (n < capacity) { memcpy(value, next, n + 1); found = 1; }
                free(next);
            }
            free(line);
            break;
        }
        free(line);
    }
    if (ferror(fp)) found = 0;
    fclose(fp);
    return found;
}

/* Write a replacement file and rename it only after successful output. */
int ini_write(const ini_file *ini, const char *key, const char *value) {
    FILE *src = NULL, *dst = NULL;
    char *temp = NULL, *line;
    size_t pathlen;
    int replaced = 0, success = 0, skip_next = 0;
    if (!ini || !ini->filename || !valid_key(key) || !valid_value(value)) return 0;
    pathlen = strlen(ini->filename);
    if (pathlen > (size_t)-1 - 5) return 0;
    temp = (char *)malloc(pathlen + 5);
    if (!temp) return 0;
    sprintf(temp, "%s.tmp", ini->filename);
    src = fopen(ini->filename, "rb");
    if (!src) {
        /* A missing file is fine; any other open error is not. */
        FILE *probe = fopen(ini->filename, "ab");
        if (!probe) goto done;
        fclose(probe);
        src = fopen(ini->filename, "rb");
        if (!src) goto done;
    }
    dst = fopen(temp, "wb");
    if (!dst) goto done;
    while ((line = read_line(src)) != NULL) {
        if (skip_next) {
            skip_next = 0;
            free(line);
            continue;
        }
        if (is_key(line, key)) {
            if (fprintf(dst, "%s\n%s\n", line, value) < 0) { free(line); goto done; }
            replaced = 1;
            skip_next = 1;
        } else if (fprintf(dst, "%s\n", line) < 0) {
            free(line);
            goto done;
        }
        free(line);
    }
    if (ferror(src)) goto done;
    if (!replaced && fprintf(dst, "[%s]\n%s\n\n", key, value) < 0) goto done;
    if (fclose(dst) != 0) { dst = NULL; goto done; }
    dst = NULL;
    if (fclose(src) != 0) { src = NULL; goto done; }
    src = NULL;
#ifdef _WIN32
    /* Windows rename does not replace an existing destination. */
    if (remove(ini->filename) != 0) goto done;
#endif
    if (rename(temp, ini->filename) != 0) goto done;
    success = 1;
done:
    if (src) fclose(src);
    if (dst) fclose(dst);
    if (!success && temp) remove(temp);
    free(temp);
    return success;
}
