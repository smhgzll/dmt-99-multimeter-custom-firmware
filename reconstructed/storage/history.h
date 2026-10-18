#ifndef DMT_HISTORY_H
#define DMT_HISTORY_H
#include <stdbool.h>
#include <stdint.h>
enum { DMT_HISTORY_CAPACITY=30 };
typedef struct {uint16_t metadata;uint16_t magnitude;} dmt_history_record;
typedef struct {uint16_t count;dmt_history_record records[DMT_HISTORY_CAPACITY];} dmt_history;
bool history_encode(uint8_t mode,uint8_t unit,float value,uint8_t overload_kind,dmt_history_record *out);
float history_decode(dmt_history_record record);
void history_push(dmt_history *history,dmt_history_record record);
#endif
