#include "util.h"

const char *Util::separator() {
#ifdef _WIN32
    return "\\";
#else
    return (char *) "/";
#endif
}


bool Util::is_integer_name(char *input) {
    size_t ln = strlen(input);
    for (size_t i = 0; i < ln; i++) {
        if (!isdigit(input[i])) {
            return false;
        }

    }
    return true;
}

size_t Util::trimwhitespace(char *out, size_t len, const char *str) {
    if (len == 0)
        return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0) // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len ? (end - str) : len;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

int Util::strcicmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char) *a) - tolower((unsigned char) *b);
        if (d != 0 || !*a)
            return d;
    }
}

int Util::copy_file(char *old_filename, char *new_filename) {
    FILE *ptr_old, *ptr_new;
    int a;

    ptr_old = fopen(old_filename, "rb");
    ptr_new = fopen(new_filename, "wb");

    if (ptr_old == 0)
        return -1;

    if (ptr_new == 0) {
        fclose(ptr_old);
        return -1;
    }

    while (1) {
        a = fgetc(ptr_old);

        if (!feof(ptr_old))
            fputc(a, ptr_new);
        else
            break;
    }

    fclose(ptr_new);
    fclose(ptr_old);
    return 0;
}

int Util::strpos(char *haystack, char *needle) {
    char *p = strstr(haystack, needle);
    if (p)
        return p - haystack;
    return -1; // Not found = -1.
}

void Util::strlower(char *dest, char *src) {
    for (int i = 0; i < strlen(src); i++) {
        dest[i] = tolower(src[i]);
    }
}

void Util::clearstr(char *str) {
    memset(str, 0, strlen(str));
}