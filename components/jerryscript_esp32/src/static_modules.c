#include <jerry-api.h>
#include <jerry-port.h>
#include <stdlib.h>
#include <string.h>
#include "jerryscript_esp32/static_modules.h"
#include "jerryscript_esp32/module_wifi.h"
#include "jerryscript_esp32/module_esp32.h"
typedef
struct {
	char *moduleName;
	ModuleConstuctor_t generatorFunc;
} static_module_t;

static static_module_t modules[] = {
	// Register the WiFi module.
	{
		.moduleName = "WiFi",
		.generatorFunc = ModuleWifi
	},
	{
		.moduleName = "HTTP",
		.generatorFunc = NULL
	},
	{
		.moduleName = "ESP32",
		.generatorFunc = ModuleESP32
	},
	// Must be last module as a terminator record.
	{
		.moduleName = NULL,
		.generatorFunc = NULL
	}
};

// End of hard coded modules

/**
 * Lookup a static module by name.
 */
ModuleConstuctor_t findStaticModule(char *moduleName) {
	static_module_t *p = modules;
	while(p->moduleName != NULL) {
		if (strcmp(moduleName, p->moduleName) == 0) {
			return p->generatorFunc;
		}
		p++;
	}
	return NULL; // No such module.
} // End of findStaticModule
