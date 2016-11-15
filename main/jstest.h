#ifndef __JSTEST_H
#define __JSTEST_H
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct {
	uint8_t *data;
	ssize_t length;
} line_record_t;

extern QueueHandle_t line_records_queue;
#endif
