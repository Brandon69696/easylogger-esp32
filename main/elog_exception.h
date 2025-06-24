#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 注册关机钩子（调用 esp_register_shutdown_handler）
 */
void elog_register_crash_hook(void);

/**
 * @brief 系统关机时的回调函数（日志落盘）
 */
void elog_system_shutdown_hook(void);

/**
 * @brief 断言失败时的钩子函数（替代 assert 默认行为）
 *
 * @param expr 失败表达式字符串
 * @param func 所在函数名
 * @param line 所在行号
 */
void elog_user_assert_hook(const char* expr, const char* func, int line);



#ifdef __cplusplus
}
#endif
