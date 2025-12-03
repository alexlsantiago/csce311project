#include "types.h"

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    char* p = (char*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (char)c;
    }
    return s;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0')
            break;
    }
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        d[i] = src[i];
    }
    for (; i < n; i++) {
        d[i] = '\0';
    }
    return dest;
}

/* =====================================================
   KERNEL-SAFE strtok IMPLEMENTATION
   ===================================================== */

static char* strtok_saved = NULL;

char* strtok(char* str, const char* delim) {
    if (str)
        strtok_saved = str;      // Start new tokenization
    else if (!strtok_saved)
        return NULL;            // No more tokens

    // Skip leading delimiters
    char* token = strtok_saved;
    while (*token) {
        int is_delim = 0;
        for (size_t i = 0; delim[i]; i++) {
            if (*token == delim[i]) {
                is_delim = 1;
                break;
            }
        }
        if (!is_delim) break;
        token++;
    }

    if (*token == '\0') {
        strtok_saved = NULL;
        return NULL;
    }

    // Token start
    char* start = token;

    // Find end of token
    while (*token) {
        for (size_t i = 0; delim[i]; i++) {
            if (*token == delim[i]) {
                *token = '\0';
                strtok_saved = token + 1;
                return start;
            }
        }
        token++;
    }

    strtok_saved = NULL;
    return start;
}

