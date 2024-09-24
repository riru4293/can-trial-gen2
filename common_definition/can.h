#ifndef CAN_H
#define CAN_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/types.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define STD_CAN_ID_MAX       ( (uint32_t)0x000007FFU )
#define EXT_CAN_ID_MAX       ( (uint32_t)0x1FFFFFFFU )
#define CAN_ID_INVAL       ( (uint32_t)0xFFFFFFFFU )

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_CAN_KIND_STD,
    E_CAN_KIND_EXT,
    E_CAN_KIND_INVAL
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

/**
 * @brief DLC definitions of CAN message.
 * 
 */
typedef enum
{
    E_CAN_DLC_MIN = 0U,
    E_CAN_DLC_MAX = E_CAN_DATA_QTY
} en_can_dlc;

typedef struct
{
    uint32_t id;
    en_can_kind kind;
    uint8_t dlc;
    uint8_t data[ E_CAN_DATA_QTY ];
} st_can_msg;

#endif /* CAN_H */
