#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <elog.h>
#include "esp_timer.h"
#include "esp_log.h"
#include <unistd.h>  // for fsync()

// 日志互斥锁
static SemaphoreHandle_t output_lock = NULL;
// 全局日志文件句柄
static FILE *log_file = NULL;

/**
 * @brief 初始化日志端口（被 elog_init() 自动调用）
 */
ElogErrCode elog_port_init(void) {
    output_lock = xSemaphoreCreateMutex();

    // 🚨务必在挂载 FATFS 之后调用 elog_init()，否则文件打不开
    log_file = fopen("/fatfs/log.txt", "w");
    if (!log_file) {
        printf("❌ Failed to open /fatfs/log.txt for writing\n");
    } else {
        printf("✅ log.txt opened successfully for writing\n");
    }

    return ELOG_NO_ERR;
}

/**
 * @brief 输出日志（写入文件）
 */
void elog_port_output(const char *log, size_t size) {
   

    // 2. 写入 FATFS 文件
    if (log_file) {
        int written = fwrite(log, 1, size, log_file);
        fflush(log_file);                     // 刷入缓冲区
        fsync(fileno(log_file));              // ⚠️ 强制写入 Flash，防止丢数据
    }
}

/**
 * @brief 输出加锁（防止多任务写冲突）
 */
void elog_port_output_lock(void) {
    xSemaphoreTake(output_lock, portMAX_DELAY);
}

/**
 * @brief 输出解锁
 */
void elog_port_output_unlock(void) {
    xSemaphoreGive(output_lock);
}

/**
 * @brief 获取当前时间戳（单位 ms）
 */
const char *elog_port_get_time(void) {
    static char time_str[24];
    int64_t now = esp_timer_get_time() / 1000;
    snprintf(time_str, sizeof(time_str), "%lld", now);
    return time_str;
}

/**
 * @brief 获取平台信息
 */
const char *elog_port_get_p_info(void) {
    return "ESP32";
}

/**
 * @brief 获取任务名信息
 */
const char *elog_port_get_t_info(void) {
    return pcTaskGetName(NULL);
}
