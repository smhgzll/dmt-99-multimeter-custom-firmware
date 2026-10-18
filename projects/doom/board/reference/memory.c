/* Freestanding libc primitives required by GCC aggregate copies. The original
 * provides memset at 0x6660 and memcpy at 0x1e59e. These portable byte loops use
 * the same non-overlap contracts; compiler helper instruction parity is not claimed.
 */
#include <stddef.h>
void *memcpy(void *dst,const void *src,size_t n)
{unsigned char *d=dst;const unsigned char *s=src;for(size_t i=0;i<n;++i)d[i]=s[i];return dst;}
void *memset(void *dst,int value,size_t n)
{unsigned char *d=dst;for(size_t i=0;i<n;++i)d[i]=(unsigned char)value;return dst;}

int memcmp(const void *a,const void *b,size_t n){const unsigned char *x=a,*y=b;while(n--){if(*x!=*y)return (int)*x-(int)*y;x++;y++;}return 0;}
char *strchr(const char *s,int c){for(;;s++){if((unsigned char)*s==(unsigned char)c)return (char *)s;if(!*s)return NULL;}}
