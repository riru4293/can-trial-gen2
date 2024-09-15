#ifndef MY_TYPES_H
#define MY_TYPES_H

#include <pico/types.h>

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef uint            UINT;
typedef uint8_t         UINT8;
typedef uint16_t        UINT16;
typedef uint32_t        UINT32;
typedef uint64_t        UINT64;
typedef bool            BOOL;
typedef void            VOID;
typedef UINT8           ERR_CD_T;
typedef size_t          SIZE_T;
typedef int             INT;

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define TRUE            ( (BOOL)true )
#define FALSE           ( (BOOL)false )

#define UINT_MAX        ( (UINT)UINT_LEAST32_MAX   )

#define E_OK            ( (ERR_CD_T)0x00U )
#define E_NOK           ( (ERR_CD_T)0xFFU )

#define WAIT_NONE       ( (TickType_t)0U )
#define WAIT_FOREVER    ( (TickType_t)portMAX_DELAY )

#endif /* MY_TYPES_H */
