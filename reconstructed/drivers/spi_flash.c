/* SPI1, PA4 CS. Recovered read transactions: 0xd4b4, 0xd516, 0xd656.
 * Program/erase translations below use an injected bus and optional timeout.
 */
#include "spi_flash.h"
uint32_t spi_flash_read_jedec_id(const dmt_spi_bus *b)
{
    b->select(b->context,true); b->transfer(b->context,0x9f);
    uint32_t v=(uint32_t)b->transfer(b->context,0xff)<<16;
    v|=(uint32_t)b->transfer(b->context,0xff)<<8;
    v|=b->transfer(b->context,0xff); b->select(b->context,false); return v;
}
uint32_t spi_flash_capacity_from_jedec(uint32_t id)
{
    /* Literal form of the compiled comparison tree at 0xd782. */
    uint32_t candidate,size;
    if(id==0xef4016)return 0x400000;
    if(id<0xef4017){
        if(id==0xef3013)return 0x80000;
        if(id<0xef3014){if(id==0xef3011)return 0x20000;candidate=0xef3012;size=0x40000;}
        else if(id==0xef4014)return 0x100000;
        else if(id<0xef4015){candidate=0xef3015;size=0x200000;}
        else return 0x200000;
    }else{
        if(id==0xef6016)return 0x400000;
        if(id<0xef6017){
            if(id==0xef4018)return 0x1000000;
            if(id<0xef4018)return 0x800000;
            candidate=0xef4019;
        }else{
            if(id==0xef6018)return 0x1000000;
            if(id<0xef6018)return 0x800000;
            candidate=0xef6019;
        }
        size=0x2000000;
    }
    if(id==candidate)return size;
    if(id==0xffffffff || id==0)return 0;
    return 0x200000;
}
uint8_t spi_flash_read_status(const dmt_spi_bus *b)
{
    b->select(b->context,true); b->transfer(b->context,5);
    uint8_t value=b->transfer(b->context,0xff); b->select(b->context,false); return value;
}
void spi_flash_read(const dmt_spi_bus *b,uint8_t *out,uint32_t a,uint16_t n)
{
    b->select(b->context,true); b->transfer(b->context,3);
    b->transfer(b->context,(uint8_t)(a>>16)); b->transfer(b->context,(uint8_t)(a>>8));
    b->transfer(b->context,(uint8_t)a);
    for(uint16_t i=0;i<n;++i) out[i]=b->transfer(b->context,0xff);
    b->select(b->context,false);
}
size_t spi_flash_page_chunk(uint32_t address,size_t remaining)
{
    /* 0xd602: first page is limited by the current 256-byte boundary. */
    size_t left=256-(address&255);return remaining<left?remaining:left;
}

void spi_flash_write_enable(const dmt_spi_bus *b)
{b->select(b->context,true);b->transfer(b->context,6);b->select(b->context,false);}
static bool wait_ready(const dmt_spi_bus *b,uint32_t budget)
{
    uint32_t polls=0;
    while(spi_flash_read_status(b)&1){if(budget && ++polls>=budget)return false;}
    return true;
}
bool spi_flash_erase_sector(const dmt_spi_bus *b,uint32_t sector,uint32_t budget)
{
    uint32_t a=sector<<12;spi_flash_write_enable(b);b->select(b->context,true);
    b->transfer(b->context,0x20);b->transfer(b->context,a>>16);b->transfer(b->context,a>>8);b->transfer(b->context,0);
    b->select(b->context,false);return wait_ready(b,budget);
}
bool spi_flash_page_program(const dmt_spi_bus *b,const uint8_t *p,uint32_t a,uint16_t n,uint32_t budget)
{
    if(n>256)n=256;
    spi_flash_write_enable(b);b->select(b->context,true);b->transfer(b->context,2);
    b->transfer(b->context,a>>16);b->transfer(b->context,a>>8);b->transfer(b->context,a);
    for(unsigned i=0;i<n;++i)b->transfer(b->context,p[i]);
    b->select(b->context,false);return wait_ready(b,budget);
}
bool spi_flash_program_pages(const dmt_spi_bus *b,const uint8_t *p,uint32_t a,uint16_t n,uint32_t budget)
{
    do{
        unsigned chunk=spi_flash_page_chunk(a,n);
        if(!spi_flash_page_program(b,p,a,chunk,budget))return false;
        if(chunk==n)return true;
        p+=chunk;a+=chunk;n-=chunk;
    }while(true);
}
bool spi_flash_write_preserve_sector(const dmt_spi_bus *b,const uint8_t *p,uint32_t a,uint16_t n,uint8_t scratch[4096],uint32_t budget)
{
    do{
        unsigned offset=a&4095,chunk=4096-offset;if(chunk>n)chunk=n;
        uint32_t base=a&~4095u;spi_flash_read(b,scratch,base,4096);
        bool erased=true;for(unsigned i=0;i<chunk;++i)if(scratch[offset+i]!=255){erased=false;break;}
        if(erased){if(!spi_flash_program_pages(b,p,a,chunk,budget))return false;}
        else{
            if(!spi_flash_erase_sector(b,a>>12,budget))return false;
            for(unsigned i=0;i<chunk;++i)scratch[offset+i]=p[i];
            if(!spi_flash_program_pages(b,scratch,base,4096,budget))return false;
        }
        if(chunk==n)return true;
        p+=chunk;a+=chunk;n-=chunk;
    }while(true);
}
