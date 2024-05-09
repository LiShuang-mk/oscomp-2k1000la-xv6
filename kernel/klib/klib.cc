#include "klib/klib.h"

// void *memset(void *s, int c, size_t n)
// {
//     unsigned char *p = (unsigned char *)s;
//     while (n--)
//         *p++ = (unsigned char)c;
//     return s;
// }

// void *memcpy(void *dest, const void *src, size_t n)
// {
//     unsigned char *d = (unsigned char *)dest;
//     const unsigned char *s = (const unsigned char *)src;
//     while (n--)
//         *d++ = *s++;
//     return dest;
// }

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d < s)
    {
        while (n--)
            *d++ = *s++;
    }
    else
    {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    while (n--)
    {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

size_t strlen(const char *s)
{
    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

char *strcat(char *dst, const char *src)
{
    size_t len = strlen(dst);
    memcpy(dst + len, src, strlen(src) + 1);
    return dst;
}

char *strcpy(char *dst, const char *src)
{
    memcpy(dst, src, strlen(src) + 1);
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    size_t len = strlen(src);
    if (len > n)
        len = n;
    memcpy(dst, src, len);
    memset(dst + len, 0, n - len);
    return dst;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s1 == *s2)
    {
        n--;
        s1++;
        s2++;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmpamb(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s2)
    {
        n--;
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}



char *strchr(const char *s, char c)
{
    while (*s)
    {
        if (*s == c)
            return (char *)s;
        s++;
    }
    return 0;
}

void snstr(char *dst, wchar const *src, int len)
{
    while (len--)
    {
        *dst++ = *src++;
    }
    *dst = 0;
}

void *memchr(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *)src_void;
    unsigned char uc = c;
    while (length--)
    {
        if (*src == uc)
            return (void *)src;
        src++;
    }
    return 0;
}

//#include <stdint.h>

float ceilf(float x) {
    union {
        float f;
        uint32 i;
    } u = {x};

    // Extract sign, exponent, and mantissa from x
    uint32 sign = u.i & 0x80000000;
    int exponent = (u.i >> 23 & 0xFF) - 127;
    uint32 mantissa = u.i & 0x7FFFFF;

    // If x is NaN, infinity, or already an integer, return x
    if (exponent >= 23 || x != x || x == 1.0f / 0.0f || x == -1.0f / 0.0f) {
        return x;
    }

    // If x is less than one and not zero, return 1.0 or 0.0 depending on the sign
    if (exponent < 0) {
        return sign ? -0.0f : 1.0f;
    }

    // If the fractional part is zero, return x
    if ((mantissa & ((1 << (23 - exponent)) - 1)) == 0) {
        return x;
    }

    // Otherwise, return the next integer toward positive infinity
    if (!sign) {
        u.i += 1 << (23 - exponent);
    }
    u.i &= ~((1 << (23 - exponent)) - 1);

    return u.f;
}
