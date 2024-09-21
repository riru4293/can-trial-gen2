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
#define CAN_DLC_MAX     ( (uint8_t)8U )
#define CAN_DLC_INVALID ( (uint8_t)UINT_LEAST8_MAX )

#define CAN_ID_INVALID  ( (uint32_t)UINT_LEAST32_MAX )

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum en_can_msg_kind
{
    E_CAN_MSG_KIND_INVALID,
    E_CAN_MSG_KIND_STD,
    E_CAN_MSG_KIND_EXT
} can_msg_kind_t;

typedef struct st_can_msg
{
    uint32_t id;
    can_msg_kind_t kind;
    uint8_t dlc;
    uint8_t data[ CAN_DLC_MAX ];
} can_msg_t;

typedef enum en_can_data_idx
{
    E_CAN_DATA_0,
    E_CAN_DATA_1,
    E_CAN_DATA_2,
    E_CAN_DATA_3,
    E_CAN_DATA_4,
    E_CAN_DATA_5,
    E_CAN_DATA_6,
    E_CAN_DATA_7
} can_data_idx_t;


#endif /* MY_CAN_H */
