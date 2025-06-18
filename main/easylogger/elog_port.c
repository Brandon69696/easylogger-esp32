#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <elog.h>
#include "esp_timer.h"
#include "esp_log.h"

static SemaphoreHandle_t output_lock = NULL;

ElogErrCode elog_port_init(void) {
    output_lock = xSemaphoreCreateMutex();
    return ELOG_NO_ERR;
}

void elog_port_output(const char *log, size_t size) {
    printf("%.*s", (int)size, log);
}

void elog_port_output_lock(void) {
    xSemaphoreTake(output_lock, portMAX_DELAY);
}

void elog_port_output_unlock(void) {
    xSemaphoreGive(output_lock);
}

const char *elog_port_get_time(void) {
    static char time_str[24];
    int64_t now = esp_timer_get_time() / 1000;
    snprintf(time_str, sizeof(time_str), "%lld", now);
    return time_str;
}

const char *elog_port_get_p_info(void) {
    return "ESP32";
}

const char *elog_port_get_t_info(void) {
    return pcTaskGetName(NULL);
}
