#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <elog.h>
#include "esp_timer.h"
#include "esp_log.h"
#include <unistd.h>           // for fsync()
#include "elog_helper.h"     // 🔁 引入共享变量声明（flash_output_lock）

// 🔐 多任务写保护锁
static SemaphoreHandle_t output_lock = NULL;

// 💾 日志文件句柄
static FILE *log_file = NULL;

// 🔄 可被 exception 使用的手动 flush 函数
void elog_flash_flush(void) {
    if (log_file) {
        fflush(log_file);             // libc 缓冲刷新
        fsync(fileno(log_file));      // 强制写入 SPI Flash
    }
}



// ✅ 初始化 port，设置日志输出文件
ElogErrCode elog_port_init(void) {
    output_lock = xSemaphoreCreateMutex();  // 初始化锁

    log_file = fopen("/fatfs/log.txt", "a");  // 追加写入日志
    if (!log_file) {
        printf("❌ Failed to open /fatfs/log.txt\n");
    } else {
        printf("✅ log.txt opened successfully\n");
    }

    return ELOG_NO_ERR;
}

// ✅ 核心日志输出函数（写入文件）
void elog_port_output(const char *log, size_t size) {
    if (flash_output_lock) return;  // 如果禁止写入则跳过

    if (log_file) {
        fwrite(log, 1, size, log_file);
        fflush(log_file);
        fsync(fileno(log_file));
    }
}

// ✅ 输出锁（在多任务系统中避免冲突）
void elog_port_output_lock(void) {
    xSemaphoreTake(output_lock, portMAX_DELAY);
}

void elog_port_output_unlock(void) {
    xSemaphoreGive(output_lock);
}

// ✅ 获取当前时间戳（ms）
const char *elog_port_get_time(void) {
    static char time_str[24];
    int64_t now = esp_timer_get_time() / 1000;
    snprintf(time_str, sizeof(time_str), "%lld", now);
    return time_str;
}

// ✅ 获取平台信息
const char *elog_port_get_p_info(void) {
    return "ESP32";
}

// ✅ 获取当前任务名
const char *elog_port_get_t_info(void) {
    return pcTaskGetName(NULL);
}
