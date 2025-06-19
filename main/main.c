#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <elog.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"
#include "sdkconfig.h"
#include <unistd.h>  // for fsync
#include <fcntl.h>   // for fileno

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
    // Step 0: Mount FS first!
    mount_fatfs_flash();

    // Step 1: stdout 设置为无缓冲（否则 printf 不及时）
    setvbuf(stdout, NULL, _IONBF, 0);

    // Step 2: 初始化 EasyLogger
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT,  ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);                   // 断言日志，显示所有信息但去掉平台信息
    elog_set_fmt(ELOG_LVL_ERROR,   ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 错误日志，显示等级/标签/时间
    elog_set_fmt(ELOG_LVL_WARN,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 警告日志
    elog_set_fmt(ELOG_LVL_INFO,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 信息日志
    elog_set_fmt(ELOG_LVL_DEBUG,   ELOG_FMT_LVL | ELOG_FMT_TAG);                       // 调试日志
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);                                      // 详细日志，显示全部字段
    elog_start();

    // Step 3: 打印一条日志（串口 + 文件）
     log_a("This is ASSERT level log");
     log_e("This is ERROR level log");
     log_w("This is WARN level log");
     log_i("🎉 This is INFO level log");
     log_d("This is DEBUG level log");
     log_v("This is VERBOSE level log");

    // Step 4: 强制刷入 Flash，等待写入完成
    vTaskDelay(pdMS_TO_TICKS(1000));  // 先等写入完成

    // Step 5: 显示 log.txt 内容
    dump_log_file();
    check_log_file_size();

  
}
