#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "jerry-api.h"
#include "jerry-port.h"
#include "sdkconfig.h"

static char tag[] = "esp32_environment";

/**
 * Implement the ESP32.status() logic.  This returns an object which contains:
 * o heap - Number of bytes of ESP32 heap remaining.
 * o sdkVersion - ESP-IDF SDK built against.
 *
 */
static jerry_value_t js_ESP32_status(
	const jerry_value_t functionObj,
	const jerry_value_t thisVal,
	const jerry_value_t args[],
	const jerry_length_t argsCount) {

	// Create the return object.
	jerry_value_t statusObject = jerry_create_object();

	// Create the heap property.
	jerry_value_t freeHeap = jerry_create_number((double)system_get_free_heap_size());
	jerry_value_t propertyName = jerry_create_string((const jerry_char_t *)"heap");
	jerry_set_property(statusObject, propertyName, freeHeap);
	jerry_release_value(freeHeap);
	jerry_release_value(propertyName);

	// Create the sdkVersion property.
	jerry_value_t sdkVersion = jerry_create_string((const jerry_char_t *)system_get_sdk_version());
	propertyName = jerry_create_string((const jerry_char_t *)"sdkVersion");
	jerry_set_property(statusObject, propertyName, sdkVersion);
	jerry_release_value(sdkVersion);
	jerry_release_value(propertyName);
	return statusObject;
} // js_ESP32_status


void setupESP32Environment() {

	// Create the ESP32 object.
	jerry_value_t ESP32Object = jerry_create_object();

	// Register the ESP32.status() function.
	jerry_value_t status = jerry_create_external_function(js_ESP32_status);
	jerry_value_t propertyName = jerry_create_string((const jerry_char_t *)"status");
	jerry_set_property(ESP32Object, propertyName, status);
	jerry_release_value(status);
	jerry_release_value(propertyName);

	// Add the ESP32 object as a global.
	jerry_value_t global_object = jerry_get_global_object();
	jerry_value_t objectName = jerry_create_string((const jerry_char_t *)"ESP32");
	jerry_set_property(global_object, objectName, ESP32Object);

	jerry_release_value(objectName);
	jerry_release_value(ESP32Object);
	jerry_release_value(global_object);
} // setupESP32Environment
