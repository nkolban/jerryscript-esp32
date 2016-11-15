/*
 * jerry_utils.c
 *
 *  Created on: Nov 13, 2016
 *      Author: kolban
 */

#include <jerry-api.h>
#include <stdlib.h>
#include <esp_log.h>
#include <jerry-port.h>
#include "sdkconfig.h"
#include "jerryscript_esp32/jerry_utils.h"

static char tag[] = "jerry_utils";


/**
 * Dump a specific property name and value.
 */
static bool dumpValue_handler (
	const jerry_value_t prop_name,
  const jerry_value_t prop_value,
  void *user_data_p)
{
	char *name = jsToString(prop_name);
	char *value = jsToString(prop_value);
	ESP_LOGD(tag, "name:%s value:%s", name!=NULL?name:"<NULL>", value!=NULL?value:"<NULL>");
	if (name != NULL) {
		free(name);
	}
	if (value != NULL) {
		free(value);
	}
	return 1;
} //End of dumpValue_handler


/**
 * Debug routine used to print the data type.
 */
void print_value(const jerry_value_t value)
{
  if (jerry_value_is_undefined (value))
  {
    jerry_port_console ("undefined");
  }
  else if (jerry_value_is_null (value))
  {
    jerry_port_console ("null");
  }
  else if (jerry_value_is_boolean (value))
  {
    if (jerry_get_boolean_value (value))
    {
      jerry_port_console ("true");
    }
    else
    {
      jerry_port_console ("false");
    }
  }
  /* Float value */
  else if (jerry_value_is_number(value))
  {
    jerry_port_console ("%f", jerry_get_number_value(value));
  }
  /* String value */
  else if (jerry_value_is_string (value))
  {
    /* Determining required buffer size */
    jerry_size_t req_sz = jerry_get_string_size (value);
    jerry_char_t str_buf_p[req_sz];

    jerry_string_to_char_buffer (value, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    jerry_port_console ("%s", (const char *) str_buf_p);
  }
  /* Object reference */
  else if (jerry_value_is_object (value))
  {
    jerry_port_console ("[JS object]");
    dumpValue(value);
  }

  jerry_port_console ("\n");
} // End of print_value


/**
 * Dump the value of a JavaScript object property by property.
 */
void dumpValue(jerry_value_t value) {
	ESP_LOGD(tag, ">> dumpValue");
	if (jerry_value_is_object(value)) {
		jerry_foreach_object_property(value, dumpValue_handler, NULL);
	} else {
		ESP_LOGD(tag, "Not an object!");
	}
	ESP_LOGD(tag, "<< dumpValue");
} // End of dumpValue


/**
 * Set the property on a passed in object and release the value.
 * Imagine we have a JavaScript object and we want to set a property on that
 * object.  We know the NULL terminated string name of the property and we
 * have a jerry_value_t representing the value.  This function will set that
 * property on the object.  It has a side effect of releasing the value of the
 * property.  Note that since we have added the value to the property this
 * would have increased its reference count and thus releasing the passed
 * in value will not result in it having no references ... it will at least
 * be referenced by the property in the object.
 */
jerry_value_t set_property_and_release(
	  jerry_value_t js_obj,  // The object to have the property added.
	  char *propertyName,    // The name of the property to add.
	  jerry_value_t js_value // The value of the property added.
	) {

	jerry_value_t js_name = jerry_create_string((const jerry_char_t *)propertyName);
	jerry_set_property(js_obj, js_name, js_value);
	jerry_release_value(js_value);
	jerry_release_value(js_name);
	return js_obj;
} // End of set_property_and_release


/**
 * Convert a jerry_value_t that represents a string to a string value.
 * The returned value is a malloced area of storage and should
 * be freed when no longer needed.
 */
char *jsToString(jerry_value_t value) {
	char *retString = NULL;
  if (jerry_value_is_number(value))
  {
  	retString = malloc(20);
  	sprintf(retString, "%f", jerry_get_number_value(value));
  } else if (jerry_value_is_string(value)) {
  	jerry_size_t strSize = jerry_get_string_size(value);
  	retString = (char *)malloc(strSize+1);
  	jerry_string_to_char_buffer(value, (jerry_char_t *)retString, strSize);
  	retString[strSize] = 0;
	}
	return retString;
} // End of jsToString
