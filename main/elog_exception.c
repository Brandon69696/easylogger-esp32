#include <elog.h>
#include <stdio.h>
#include <unistd.h>
#include "elog_exception.h"
#include "esp_system.h"
#include "elog_helper.h"  // ✅ 用共享变量
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern void elog_flash_flush(void);
extern void dump_log_file(void);
extern void check_log_file_size(void);

void elog_user_assert_hook(const char* expr, const char* func, int line) {
    flash_output_lock = false;

    elog_a("ASSERT", "❌ (%s) failed at %s:%d", expr, func, line);
    elog_flash_flush();

    // ✅ 在重启前打印日志文件内容
    dump_log_file();
    check_log_file_size();

    vTaskDelay(pdMS_TO_TICKS(1000));  // 给 UART / 文件系统 1 秒缓冲时间
    esp_restart();  // 🔁 最后再重启
}


void elog_register_crash_hook() {
    esp_register_shutdown_handler(elog_system_shutdown_hook);
}

void elog_system_shutdown_hook() {
    flash_output_lock = false;
    elog_e("CRASH", "🔥 shutdown_hook triggered before reset");
    elog_flash_flush();
}
