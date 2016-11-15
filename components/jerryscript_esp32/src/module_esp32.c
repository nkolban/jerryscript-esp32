#include <jerry-api.h>
#include <esp_system.h>
#include "jerryscript_esp32/module_esp32.h"
#include "jerryscript_esp32/jerry_utils.h"

/**
 * ESP32.getState()
 * Return an object that describes the state of the ESP32 environment.
 * - heapSize - The available heap size
 */
static jerry_value_t js_esp32_getState_func (
  const jerry_value_t functionObj,
  const jerry_value_t thisVal,
  const jerry_value_t args[],
  const jerry_length_t argsCount) {

	jerry_value_t js_retValue;

	js_retValue = jerry_create_object();
	set_property_and_release(js_retValue, "heapSize", jerry_create_number((double)system_get_free_heap_size()));
	return js_retValue;
} // End of js_esp32_getState_func


/**
 * Create the module that creates the ESP32 handler.
 */
jerry_value_t ModuleESP32() {
	jerry_value_t js_retObj;
	js_retObj = jerry_create_object();
	set_property_and_release(js_retObj, "getState", jerry_create_external_function(js_esp32_getState_func));
	return js_retObj;
} // End of ModuleESP32
