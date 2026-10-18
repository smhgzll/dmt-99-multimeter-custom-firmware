#ifndef DMT_FORMAT_H
#define DMT_FORMAT_H
#include <stddef.h>
#include <stdint.h>
/* Bounded native form of 0x910e/0x969e for the recovered application formats.
 * Returns bytes including NUL; zero means unsupported numeric extent/truncation.
 */
size_t dmt_format(char *,size_t,const char *,...);
void measurement_format_value(char *,size_t,float,uint8_t mode,uint8_t unit);
#endif
