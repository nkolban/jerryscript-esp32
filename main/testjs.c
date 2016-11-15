#include <string.h>
#include <esp_log.h>
#include <jerry-api.h>
#include <jerry-port.h>
#include <rom/uart.h>
#include "telnet.h"
#include "jstest.h"
#include "jerryscript_esp32/jerryscript_esp32.h"
#include "jerryscript_esp32/jerry_utils.h"

QueueHandle_t line_records_queue;

extern void jerry_port_esp32_setConsole(int (*consoleCallbackParam)(const char *fmt, va_list va));
extern void setupESP32Environment();

//static char tag[] = "testjs";



/**
 * A function used to read a line of data from the UART
 * attached serial port.
 */
/*
size_t readLineFromUART(char *buffer, int maxLen) {
	char *tail = buffer;
	size_t len = 0;
	while(1) {
		STATUS status = uart_rx_one_char((uint8_t*)tail);
		if (status != OK) {
			continue;
		}
		if (*tail < 32) {
			//ESP_LOGI(tag, "Got a char: nonprint - 0x%x", *cmd_tail);
		} else {
			//ESP_LOGI(tag, "Got a char: %c - 0x%x", *cmd_tail, *cmd_tail);
		}

		uart_tx_one_char(*tail);
		uart_tx_flush(0);
		if (*tail == '\r' || *tail == '\n'){
			break;
		}
		tail++;
		len++;
		if (len == maxLen) {
			break;
		}
	} // End loop for line
	return len;
} // readLineFromUART
*/


/**
 * Read a line of text from the queue of line records and blocking if none
 * are available.  The line is copied into the passed in buffer and the
 * length of the line read is returned.  How the line is added to the queue
 * is of no importance to the caller of this function.
 */
size_t readLine(char *buffer, int maxLen) {
	line_record_t line_record;
	size_t retSize;
	BaseType_t rc = xQueueReceive(line_records_queue, &line_record, portMAX_DELAY);
	if (rc == pdTRUE) {
		if (line_record.length > maxLen) {
			line_record.length = maxLen;
		}
		memcpy(buffer, line_record.data, line_record.length);
		free(line_record.data);
		retSize = line_record.length;
	} else {
		retSize = 0;
	}
	return retSize;
} // End of readLine


/**
 * A callback function that is invoked when JerryScript wishes to
 * write something to the console.  In the implementation we send
 * the line to any attached telnet client.
 */
int consoleCallback(const char *fmt, va_list va) {
	telnet_esp32_vprintf(fmt, va);
	return 0;
} // consoleCallback


/**
 * Test the JerryScript environment.
 */
int testjs()
{
	jerry_init(JERRY_INIT_EMPTY);

	setupESP32Environment();

	jerryscript_esp32_init();

	// Set the callback function to be invoked when JerryScript wishes to
	// send data to the console.
	jerry_port_esp32_setConsole(consoleCallback);

	while(1) {
		char cmd[256] = {};
		size_t len = 0;

		jerry_port_console ("> ");
		len = readLine(cmd, sizeof(cmd));
		//ESP_LOGI(tag, "Got a line: %.*s", len, cmd);
		uart_tx_one_char('\n');
		jerry_value_t ret_val;
		ret_val = jerry_eval ((const jerry_char_t *) cmd,
													len,
													0);
		if (jerry_value_has_error_flag (ret_val))
		{
			/* Evaluated JS code thrown an exception
			 *  and didn't handle it with try-catch-finally */
			jerry_port_console ("Unhandled JS exception occured: ");
		}
		print_value (ret_val);
		jerry_release_value (ret_val);
	}
} // End of testjs

double __ieee754_remainder(double x, double p) {
	return 0.0;
}
