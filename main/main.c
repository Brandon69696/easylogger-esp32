#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <elog.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"
#include "esp_task_wdt.h"
#include "sdkconfig.h"
#include <unistd.h>  // for fsync
#include <fcntl.h>   // for fileno
#include "elog_exception.h"  // 你的异常hook
#include "elog_helper.h"    // ✅ 用于访问 flash_output_lock
#include "my_assert.h"       // 你的断言宏



#define LOG_FILE_PATH "/fatfs/log.txt"
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static const char *TAG = "MAIN";

// Step 1: 挂载 FATFS 到 SPI Flash
void mount_fatfs_flash() {
    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .use_one_fat = false
    };

    esp_err_t ret = esp_vfs_fat_spiflash_mount_rw_wl("/fatfs", "storage", &mount_config, &s_wl_handle);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "✅ FATFS mounted at /fatfs");
    } else {
        ESP_LOGE(TAG, "❌ Failed to mount FATFS: %s", esp_err_to_name(ret));
    }
}

// Step 2: 打印 log.txt 内容（可选）
void dump_log_file() {
    FILE *f = fopen(LOG_FILE_PATH, "r");
    if (!f) {
        printf("❌ Cannot open %s\n", LOG_FILE_PATH);
        return;
    }

    printf("\n--- 📄 Dumping log.txt ---\n");
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    printf("--- ✅ End of log.txt ---\n");
}

// Step 3: 检查 log.txt 文件大小
void check_log_file_size() {
    struct stat st;
    if (stat(LOG_FILE_PATH, &st) == 0) {
        printf("📏 log.txt size: %ld bytes\n", st.st_size);
    } else {
        printf("❌ stat() failed on log.txt\n");
    }
}

void app_main(void) {
    // esp_task_wdt_delete(xTaskGetCurrentTaskHandle());  // ✅ 删除当前任务的 watchdog，防止报错

    // Step 0: 挂载 Flash 文件系统
    mount_fatfs_flash();

    // Step 1: 设置 stdout 为无缓冲，确保 crash 时能立即打印
    setvbuf(stdout, NULL, _IONBF, 0);

    // Step 2: 初始化 EasyLogger
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT,  ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_ERROR,   ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG,   ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);
    elog_start();

    // Step 3: 关闭写锁，注册系统异常钩子
    flash_output_lock = false;
    elog_register_crash_hook();


    // Step 5: 触发断言（可注释）
    int *p = NULL;
    MY_ASSERT(p != NULL);  // 会触发异常钩子，写入日志并停机

    // Step 6: 可选，查看日志内容
  
   
}
