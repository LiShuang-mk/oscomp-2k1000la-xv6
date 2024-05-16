#include "klib/klib.h"
#include "klib/global_operator.hh"
// void *memset(void *s, int c, size_t n) {
//   for(char *b=(char*)s;b-(char*)s<n;b++)*b=c;
//   return s;
// }


void *memmove(void *dst, const void *src, size_t n) {
  // may overlap
  if(n<=0)return dst;
  const char *i=(const char*)src;
  char *j=(char*)dst;
  for(i+=n-1,j+=n-1;i>=(const char*)src;i--,j--)*j=*i;
  return dst;
}


void *memcpy(void *out, const void *in, size_t n) {
    const char *i=(const char*)in;
    char *j=(char*)out;
    for(; static_cast<size_t>(j-((char*)out))<n; i++,j++)*j=*i;
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t cnt=1;
  const unsigned char *i=(const unsigned char*)s1, *j=(const unsigned char*)s2;
  if(n==0)return 0;
  for(;*i==*j;i++,j++,cnt++){
    if(!*i||cnt>=n)return 0;
  }
  return *i<*j?-1:1;
}

const void *
memchr (const void *src_void,
	int c,
	size_t length)
{
  const unsigned char *src = (const unsigned char *) src_void;
  unsigned char d = c;

  while (length--)
    {
      if (*src == d)
        return (void *) src;
      src++;
    }

  return nullptr;
}

// convert wide char string into uchar string 
void snstr(char *dst, wchar const *src, int len) {
  while (len -- && *src) {
    *dst++ = (uchar)(*src & 0xff);
    src ++;
  }
  while(len-- > 0)
    *dst++ = 0;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *j;
  size_t cnt=0;
  for(j=dst;*src&&cnt<n;src++,j++,cnt++)*j=*src;
  for(;cnt<n;j++,cnt++)*j='\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *j=dst;
  while(*j)j++;
  for(;*src;src++,j++)*j=*src;
  *j='\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  for(;*s1==*s2;s1++,s2++){
    if(!*s1)return 0;
  }
  return *s1<*s2?-1:1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t cnt=1;
  if(n==0)return 0;
  for(;*s1==*s2;s1++,s2++,cnt++){
    if(!*s1||cnt==n)return 0;
  }
  return *s1<*s2?-1:1;
}

int strncmpamb(const char *s1, const char *s2, size_t n) {
  size_t cnt=1;
  if(n==0)return 0;
  for(;(*s1==*s2)||((*s1>='a')&&(*s1<='z')&&((*s2-*s1)==('A'-'a')))||((*s1>='A')&&(*s1<='Z')&&((*s1-*s2)==('A'-'a')));s1++,s2++,cnt++){
    if(!*s1||cnt==n)return 0;
  }
  return *s1<*s2?-1:1;
}

char* strchr(const char *s, char c) {
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char *strcpy(char *dst, const char *src) {
  char *j;
  for(j=dst;*src;src++,j++)*j=*src;
  *j='\0';
  return dst;
}

size_t strlen(const char *s) {
  size_t len=0;
  while(*s)s++,len++;
  return len;
}

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

void*  operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return operator new (size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
  // currently we don't support alignment 
  return operator new(size);
}