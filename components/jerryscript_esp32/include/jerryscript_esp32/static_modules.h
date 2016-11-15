#ifndef COMPONENTS_JERRYSCRIPT_ESP32_INCLUDE_JERRYSCRIPT_ESP32_STATIC_MODULES_H_
#define COMPONENTS_JERRYSCRIPT_ESP32_INCLUDE_JERRYSCRIPT_ESP32_STATIC_MODULES_H_

#include <jerry-api.h>
typedef jerry_value_t (*ModuleConstuctor_t)();
ModuleConstuctor_t findStaticModule(char *moduleName);
#endif
