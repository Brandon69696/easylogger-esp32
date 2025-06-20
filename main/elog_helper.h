#ifndef __ELOG_HELPER_H__
#define __ELOG_HELPER_H__
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

// 🔓 共享变量：控制是否允许写 Flash
extern bool flash_output_lock;

// 🔧 控制函数：启用/禁用日志写入


#ifdef __cplusplus
}
#endif

#endif  // __ELOG_HELPER_H__
