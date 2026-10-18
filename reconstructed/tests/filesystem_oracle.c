/* Test-only exports of upstream routines for original-instruction comparisons. */
#include "../vendor/fatfs/ff.c"
DWORD oracle_ld(const BYTE *p){return ld_dword(p);}
void oracle_st(BYTE *p,DWORD v){st_dword(p,v);}
BYTE oracle_checksum(const BYTE *p){return sum_sfn(p);}
DWORD oracle_cluster_sector(DWORD base,WORD size,DWORD count,DWORD c){FATFS fs={0};fs.database=base;fs.csize=size;fs.n_fatent=count;return clst2sect(&fs,c);}
int oracle_drive(const TCHAR **p){return get_ldnumber(p);}
UINT oracle_check(FATFS *fs,LBA_t s){return check_fs(fs,s);}
DWORD oracle_load_cluster(BYTE type,const BYTE *directory){FATFS fs={0};fs.fs_type=type;return ld_clust(&fs,directory);}
void oracle_store_cluster(BYTE type,BYTE *directory,DWORD cluster){FATFS fs={0};fs.fs_type=type;st_clust(&fs,directory,cluster);}
