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
typedef enum
{
    E_CAN_KIND_STD,
    E_CAN_KIND_EXT
} en_can_kind;

typedef enum
{
    E_CAN_DATA_1,
    E_CAN_DATA_2,
    E_CAN_DATA_3,
    E_CAN_DATA_4,
    E_CAN_DATA_5,
    E_CAN_DATA_6,
    E_CAN_DATA_7,
    E_CAN_DATA_8,
    E_CAN_DATA_QTY
} en_can_data;

typedef struct
{
    uint32_t id;
    en_can_kind kind;
    uint8_t dlc;
    uint8_t data[ E_CAN_DATA_QTY ];
} st_can_msg;

#endif /* MY_CAN_H */
