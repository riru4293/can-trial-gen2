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
typedef enum en_can_frame_kind
{
    E_CAN_FRAME_INVALID,
    E_CAN_FRAME_DATA,
    E_CAN_FRAME_REMOTE
} can_frame_kind_t;

typedef struct st_can_frame
{
    uint32_t id;
    can_frame_kind_t kind;
    uint8_t dlc;
    uint8_t data[ CAN_DLC_MAX ];
} can_frame_t;

#endif /* MY_CAN_H */
