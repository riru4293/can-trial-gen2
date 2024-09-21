#ifndef MY_CAN_H
#define MY_CAN_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/types.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum can_kind
{
    E_CAN_KIND_STD,
    E_CAN_KIND_EXT
} en_can_kind_t;

typedef enum can_data
{
    E_CAN_DATA_1 = 0U,
    E_CAN_DATA_2,
    E_CAN_DATA_3,
    E_CAN_DATA_4,
    E_CAN_DATA_5,
    E_CAN_DATA_6,
    E_CAN_DATA_7,
    E_CAN_DATA_8,
    E_CAN_DATA_QTY
} en_can_data_t;

typedef struct can_msg
{
    uint32_t id;
    en_can_kind_t kind;
    uint8_t dlc;
    uint8_t data[ E_CAN_DATA_QTY ];
} st_can_msg_t;

#endif /* MY_CAN_H */
