#include <esp_log.h>
#include <stdlib.h>
#include <jerry-api.h>
#include "sdkconfig.h"
#include "jerryscript_esp32/require.h"
#include "jerryscript_esp32/curl_client.h"
#include "jerryscript_esp32/jerry_utils.h"

static char tag[] = "jerryscript_esp32";

static jerry_value_t js_console_log_func (
  const jerry_value_t functionObj,
  const jerry_value_t thisVal,
  const jerry_value_t args[],
  const jerry_length_t argsCount) {
	ESP_LOGD(tag, "console.log(...)");
	return 0;
} // End of js_console_log_func
/**
 * Add the console handler.
 */
static void register_console() {
	ESP_LOGV(tag, ">> register_console");
	jerry_value_t js_console = jerry_create_object();
	set_property_and_release(js_console,
		"log", jerry_create_external_function(js_console_log_func));

	jerry_value_t global_object = jerry_get_global_object();
	set_property_and_release(global_object,
		"console", js_console);
	jerry_release_value(global_object);
	ESP_LOGV(tag, "<< register_console");
} // End of jerryscript_esp32_register_requires


/**
 * Initialize the jerryscript_esp32 environment.
 */
void jerryscript_esp32_init() {
	// Add the "requires()" global.
	jerryscript_esp32_register_requires();
	jerryscript_esp32_register_load();

	register_console();
} // jerryscript_esp32_init
