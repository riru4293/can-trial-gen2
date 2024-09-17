#ifndef MY_TASK_CFG_H
#define MY_TASK_CFG_H

#include <pico/types.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum en_task_priority
{
    E_TASK_PRIO_MIN = 1U,
    E_TASK_PRIO_CAN = 5U,
    E_TASK_PRIO_MAX = 8U

} task_priority_t;

#endif /* MY_TASK_CFG_H */
