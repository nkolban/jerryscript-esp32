/*
 * curl_client.c
 *
 *  Created on: Nov 13, 2016
 *      Author: kolban
 *
 * Web:
 * libcurl - https://curl.haxx.se/libcurl/
 * CURL examples - https://curl.haxx.se/libcurl/c/example.html
 */

#include <esp_log.h>
#include <curl/curl.h>
#include <jerry-api.h>
#include <string.h>
#include <stddef.h>
#include <freertos/event_groups.h>
#include "jerryscript_esp32/jerry_utils.h"
#include "jerryscript_esp32/curl_client.h"
#include "sdkconfig.h"

static char tag[] = "curl_client";

typedef struct {
	size_t size;
	uint8_t *data;
} context_t;
/**
 * Receive data from a CURL callback.
 */
static size_t writeData(void *buffer, size_t size, size_t nmemb, void *userp) {
	ESP_LOGD(tag, "writeData: size=%d, number=%d", size, nmemb);
	ESP_LOGD(tag, "data: %.*s",size*nmemb, (char *)buffer);
	size_t total = size * nmemb;
	context_t *context = (context_t *)userp;
	if (context->size == 0) {
		context->data = malloc(total);
	} else {
		context->data = realloc(context->data, context->size + total);
	}
	memcpy(context->data + context->size, buffer, total);
	context->size += total;
	return total;
}
/**
 * Get the content from a CURL client request to a remote
 * URL.  The return is a NULL terminated string that contains
 * that content.  If an error occurred or the file was not found
 * on the server, then NULL is returned.
 */
char *curl_client_getContent(char *url) {
	CURL *curlHandle;
	CURLcode res;
	char *retData = NULL;
	ESP_LOGD(tag, ">> curl_client: %s", url);

	curlHandle = curl_easy_init();
	if(curlHandle) {
		context_t *context = (context_t *)malloc(sizeof(context_t));
		context->size = 0;
		curl_easy_setopt(curlHandle, CURLOPT_URL, url);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
		// Set the callback function
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, context);
		curl_easy_setopt(curlHandle, CURLOPT_FAILONERROR, 1);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curlHandle);
		ESP_LOGD(tag, "curl_easy_perform: rc=%d, size=%d", res, context->size);
		/* Check for errors */
		if(res != CURLE_OK) {
			ESP_LOGE(tag, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
		}

		/* always cleanup */
		curl_easy_cleanup(curlHandle);
		if (res == CURLE_OK && context->size > 0) {
			retData = malloc(context->size + 1);
			memcpy(retData, context->data, context->size);
			retData[context->size] = 0;
		}
		if (context->data != NULL) {
			free(context->data);
		}
		free(context);
	}
	ESP_LOGD(tag, "<< curl_client");
	return retData;
} // End of curl_client_getContent

/**
 * Implement the JavaScript load function.
 * Within JavaScript, we will be able to run:
 *
 * load(String)
 *
 * where the string can be treated as a URL.  The semantics of the function
 * will be to retrieve the content at the URL and "run it" in the context
 * of our JavaScript environment.
 */
static jerry_value_t js_load_func (
  const jerry_value_t functionObj,
  const jerry_value_t thisVal,
  const jerry_value_t args[],
  const jerry_length_t argsCount) {

	ESP_LOGD(tag, ">> load");
	if (argsCount == 0) {
		ESP_LOGE(tag, "No parameter supplied for load");
		ESP_LOGD(tag, "<< load");
		return 0;
	}
	char *url = jsToString(args[0]);
	if (url == NULL) {
		ESP_LOGE(tag, "Couldn't make a URL string");
		ESP_LOGD(tag, "<< load");
		return 0;
	}
	ESP_LOGD(tag, "target URL is %s", url);
	char *data = curl_client_getContent(url);
	ESP_LOGD(tag, "We have data!! 0x%lx", (unsigned long)data);
	if (data != NULL) {
		ESP_LOGD(tag, "%s", data);
		// Here we have the data returned from the web server.  Now we
		// need to do something with it.
		jerry_value_t js_runnable = jerry_parse((jerry_char_t *)data, strlen(data), 0);
		jerry_run(js_runnable);
		jerry_release_value(js_runnable);
		//jerry_eval((jerry_char_t *)data, strlen(data), 0);

		free(data);
	}
	free(url);
	ESP_LOGD(tag, "<< load");
	return 0;
}

/**
 * Register the load function.  This means that a function called
 * load() will be added to the global context of our JavaScript environment.
 */
void jerryscript_esp32_register_load()
{
  ESP_LOGV(tag, ">> jerryscript_esp32_register_load");
  jerry_value_t global_object = jerry_get_global_object();
  set_property_and_release(global_object,
  	"load", jerry_create_external_function(js_load_func));
  jerry_release_value(global_object);
  ESP_LOGV(tag, "<< jerryscript_esp32_register_load");
} // End of jerryscript_esp32_register_requires
