#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <elog.h>

void app_main(void) {
    // 关闭 stdout 缓冲区，确保立即输出日志
    setvbuf(stdout, NULL, _IONBF, 0);

    // 初始化 EasyLogger
    elog_init();

    // 配置不同等级的日志格式（可自由调整）
    elog_set_fmt(ELOG_LVL_ASSERT,  ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);                   // 断言日志，显示所有信息但去掉平台信息
    elog_set_fmt(ELOG_LVL_ERROR,   ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 错误日志，显示等级/标签/时间
    elog_set_fmt(ELOG_LVL_WARN,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 警告日志
    elog_set_fmt(ELOG_LVL_INFO,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);       // 信息日志
    elog_set_fmt(ELOG_LVL_DEBUG,   ELOG_FMT_LVL | ELOG_FMT_TAG);                       // 调试日志
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);                                      // 详细日志，显示全部字段

    // 启动 EasyLogger
    elog_start();

    // 多级别日志输出测试
    while (1) {
        log_a("This is ASSERT level log");
        log_e("This is ERROR level log");
        log_w("This is WARN level log");
        log_i("🎉 This is INFO level log");
        log_d("This is DEBUG level log");
        log_v("This is VERBOSE level log");

        vTaskDelay(pdMS_TO_TICKS(2000)); // 延迟 2 秒
    }
}
