#ifndef CDF_CAN_H
#define CDF_CAN_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* RP2040 */
#include <pico/types.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define CDF_STD_CAN_ID_MAX       ( (uint32_t)0x000007FFU )
#define CDF_EXT_CAN_ID_MAX       ( (uint32_t)0x1FFFFFFFU )
#define CDF_CAN_ID_INVALID       ( (uint32_t)0xFFFFFFFFU )

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_CDF_CAN_KIND_STD,
    E_CDF_CAN_KIND_EXT,
    E_CDF_CAN_KIND_INVALID
} en_cdf_can_kind;

typedef enum
{
    E_CDF_CAN_DATA_1,
    E_CDF_CAN_DATA_2,
    E_CDF_CAN_DATA_3,
    E_CDF_CAN_DATA_4,
    E_CDF_CAN_DATA_5,
    E_CDF_CAN_DATA_6,
    E_CDF_CAN_DATA_7,
    E_CDF_CAN_DATA_8,
    E_CDF_CAN_DATA_QTY
} en_cdf_can_data;

typedef enum
{
    E_CDF_CAN_DLC_MIN = 0U,
    E_CDF_CAN_DLC_MAX = E_CDF_CAN_DATA_QTY
} en_cdf_can_dlc;

typedef struct
{
    uint32_t id;
    en_cdf_can_kind kind;
    uint8_t dlc;
    uint8_t data[ E_CDF_CAN_DATA_QTY ];
} st_cdf_can_msg;

#endif /* CDF_CAN_H */
