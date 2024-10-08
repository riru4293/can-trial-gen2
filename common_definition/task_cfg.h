#ifndef TASK_CFG_H
#define TASK_CFG_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_TASK_PRIO_MIN = 1U,
    E_TASK_PRIO_CAN_TX = 2U,
    E_TASK_PRIO_CAN = 5U,
    E_TASK_PRIO_MAX = 8U

} en_task_prio;

#endif /* TASK_CFG_H */
