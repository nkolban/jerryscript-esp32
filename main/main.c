#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <lwip/sockets.h>
#include <string.h>
#include "jstest.h"
#include "telnet.h"

static char tag[]="main";

extern int testjs();

// The IP address that we want our device to have.
#define DEVICE_IP          "192.168.5.2"

// The Gateway address where we wish to send packets.
// This will commonly be our access point.
#define DEVICE_GW          "192.168.5.1"

// The netmask specification.
#define DEVICE_NETMASK     "255.255.255.0"

// The identity of the access point to which we wish to connect.
#define AP_TARGET_SSID     "RASPI3"

// The password we need to supply to the access point for authorization.
#define AP_TARGET_PASSWORD "password"


static void recvData(uint8_t *buffer, size_t size) {
	//ESP_LOGD(tag, "We received: %.*s", size, buffer);
	// We have received a line of data from the telnet client, now we want
	// to present it to JerryScript for processing.
	line_record_t line_record;
	line_record.length = size;
	line_record.data = malloc(size);
	memcpy(line_record.data, buffer, size);
	xQueueSendToBack(line_records_queue, &line_record, portMAX_DELAY);
}

static void telnetTask(void *data) {
	ESP_LOGD(tag, ">> telnetTask");
	telnet_esp32_listenForClients(recvData);
	ESP_LOGD(tag, "<< telnetTask");
	vTaskDelete(NULL);
}

static void jsTask(void *data) {
  testjs();
  vTaskDelete(NULL);
}

static esp_err_t wifiEventHandler(void *ctx, system_event_t *event)
{
	if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
		xTaskCreatePinnedToCore(&telnetTask, "telnetTask", 8048, NULL, 5, NULL, 0);
	}
  return ESP_OK;
}


static void init() {
	line_records_queue = xQueueCreate(10, sizeof(line_record_t));
}

int app_main(void)
{
	nvs_flash_init();
	tcpip_adapter_init();

	tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA); // Don't run a DHCP client
	tcpip_adapter_ip_info_t ipInfo;

	inet_pton(AF_INET, DEVICE_IP, &ipInfo.ip);
	inet_pton(AF_INET, DEVICE_GW, &ipInfo.gw);
	inet_pton(AF_INET, DEVICE_NETMASK, &ipInfo.netmask);
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);

	init();
	ESP_ERROR_CHECK(esp_event_loop_init(wifiEventHandler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	wifi_config_t sta_config = {
		.sta = {
			.ssid      = AP_TARGET_SSID,
			.password  = AP_TARGET_PASSWORD,
			.bssid_set = 0
		}
	};
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());

	xTaskCreatePinnedToCore(&jsTask, "jsTask", 8048, NULL, 5, NULL, 0);
	return 0;
}

