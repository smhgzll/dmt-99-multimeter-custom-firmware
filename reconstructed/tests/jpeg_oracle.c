/* Test-only access to upstream static routines and MCU stream. */
#include <stddef.h>
#include <string.h>
#include "../vendor/tjpgd/tjpgd.c"
typedef struct {const BYTE *bytes;UINT size,offset;WORD *pixels;UINT capacity,count,rectangles;} stream;
static UINT input(JDEC *j,BYTE *out,UINT n){stream *s=j->device;if(n>s->size-s->offset)n=s->size-s->offset;if(out)memcpy(out,s->bytes+s->offset,n);s->offset+=n;return n;}
static UINT output(JDEC *j,void *pixels,JRECT *r){stream *s=j->device;UINT n=(r->right-r->left+1)*(r->bottom-r->top+1);if(s->count+n>s->capacity)return 0;memcpy(s->pixels+s->count,pixels,n*2);s->count+=n;s->rectangles++;return 1;}
int phase3_jpeg_decode(const BYTE *data,UINT size,WORD *pixels,UINT capacity,UINT *info){
 JDEC j;union{uint32_t align;BYTE bytes[0x1c00];}pool;stream s={data,size,0,pixels,capacity,0,0};
 int p=jd_prepare(&j,input,pool.bytes,sizeof(pool.bytes),&s);info[0]=p;info[1]=s.offset;
 if(p)return p;
 info[2]=j.width;info[3]=j.height;info[4]=sizeof(pool.bytes)-j.sz_pool;
 int d=jd_decomp(&j,output,0);info[5]=d;info[6]=s.count;info[7]=s.rectangles;return d;
}
UINT phase3_pool_size(void){return sizeof(JDEC);}
