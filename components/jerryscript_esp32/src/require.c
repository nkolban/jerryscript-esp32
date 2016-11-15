#include <esp_log.h>
#include <jerry-api.h>
#include <jerry-port.h>
#include <stdlib.h>
#include <string.h>

#include "sdkconfig.h"

#include "jerryscript_esp32/jerry_utils.h"
#include "jerryscript_esp32/require.h"
#include "jerryscript_esp32/static_modules.h"

static char tag[] = "require";


/**
 * The js_require_func implements the global require() function.  This function
 * takes as input a String parameter which represents the name of the module to be
 * loaded.  The return is an object that represents the module.  If the module is not
 * found or the input parameters to the function are invalid, we return a null.
 */
static jerry_value_t js_require_func (
  const jerry_value_t functionObj,
  const jerry_value_t thisVal,
  const jerry_value_t args[],
  const jerry_length_t argsCount) {

	jerry_value_t retValue;


  ESP_LOGD(tag, "js_require called! - args count is %d", argsCount);

  if (argsCount == 0) {
  	ESP_LOGW(tag, "No module name supplied");
  	return jerry_create_null();
  }

  if (jerry_value_is_string(args[0]) == 0) {
  	ESP_LOGW(tag, "No module name supplied as string");
  	return jerry_create_null();
  }

  char *moduleName = jsToString(args[0]); // The module name is the first argument.

  ModuleConstuctor_t moduleConstructor = findStaticModule(moduleName);
  if (moduleConstructor == NULL) {
  	ESP_LOGD(tag, "Failed to find module by name - %s", moduleName);
  	retValue = jerry_create_null(); // Return a null.
  } else {
  	retValue = moduleConstructor(); // Invoke the module moduleConstructor.
  }

  free(moduleName);
  return retValue;
} // js_require_func


/**
 * Register the require() global function.  This adds the function called
 * require() to the global namespace.
 */
void jerryscript_esp32_register_requires()
{
  ESP_LOGV(tag, ">> jerryscript_esp32_register_requires");
  jerry_value_t global_object = jerry_get_global_object();
  set_property_and_release(global_object,
  	"require", jerry_create_external_function(js_require_func));
  jerry_release_value(global_object);
  ESP_LOGV(tag, "<< jerryscript_esp32_register_requires");
} // End of jerryscript_esp32_register_requires
