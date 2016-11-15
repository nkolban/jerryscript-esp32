#include <jerry-api.h>
#include <esp_wifi.h>
#include <esp_err.h>
#include "jerryscript_esp32/module_wifi.h"
#include "jerryscript_esp32/jerry_utils.h"

/**
 * wifi.getState()
 * Return an object that describes the state of the WiFi environment.
 * - boolean isStation
 * - boolean isAccessPoint
 */
static jerry_value_t js_wifi_getState_func (
  const jerry_value_t functionObj,
  const jerry_value_t thisVal,
  const jerry_value_t args[],
  const jerry_length_t argsCount) {

	jerry_value_t js_retValue;
	jerry_value_t js_isStation;
	jerry_value_t js_isAcessPoint;

	wifi_mode_t mode;

	ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
	switch(mode) {
	case WIFI_MODE_NULL:
		js_isAcessPoint = jerry_create_boolean(0);
		js_isStation = jerry_create_boolean(0);
		break;
	case WIFI_MODE_STA:
		js_isAcessPoint = jerry_create_boolean(0);
		js_isStation = jerry_create_boolean(1);
		break;
	case WIFI_MODE_AP:
		js_isAcessPoint = jerry_create_boolean(1);
		js_isStation = jerry_create_boolean(0);
		break;
	case WIFI_MODE_APSTA:
		js_isAcessPoint = jerry_create_boolean(1);
		js_isStation = jerry_create_boolean(1);
		break;
	default:
		js_isAcessPoint = jerry_create_boolean(0);
		js_isStation = jerry_create_boolean(0);
		break;
	}

	js_retValue = jerry_create_object();
	set_property_and_release(js_retValue, "isStation", js_isStation);
	set_property_and_release(js_retValue, "isAccessPoint", js_isAcessPoint);
	return js_retValue;
} // End of js_wifi_getState_func


/**
 * Create the module that creates the WiFI handler.
 */
jerry_value_t ModuleWifi() {
	jerry_value_t js_retObj;
	js_retObj = jerry_create_object();
	set_property_and_release(js_retObj, "getState", jerry_create_external_function(js_wifi_getState_func));
	return js_retObj;
} // End of ModuleWiFi
