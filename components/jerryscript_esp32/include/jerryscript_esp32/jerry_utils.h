/*
 * jerry_utils.h
 *
 *  Created on: Nov 13, 2016
 *      Author: kolban
 */

#ifndef COMPONENTS_JERRYSCRIPT_ESP32_INCLUDE_JERRYSCRIPT_ESP32_JERRY_UTILS_H_
#define COMPONENTS_JERRYSCRIPT_ESP32_INCLUDE_JERRYSCRIPT_ESP32_JERRY_UTILS_H_

#include <jerry-api.h>

jerry_value_t set_property_and_release(
	jerry_value_t js_obj,
	char *propertyName,
	jerry_value_t js_value);
char *jsToString(jerry_value_t stringValue);
void dumpValue(jerry_value_t value);
void print_value (const jerry_value_t value);

#endif /* COMPONENTS_JERRYSCRIPT_ESP32_INCLUDE_JERRYSCRIPT_ESP32_JERRY_UTILS_H_ */
