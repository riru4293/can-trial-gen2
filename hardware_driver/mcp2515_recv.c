/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

/* My standard library */
#include <my_error_code.h>

/* Driver */
#include <private/mcp2515.h>
#include <private/mcp2515_spicmd.h>
#include <private/mcp2515_register.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum en_can_buff
{
    E_CAN_BUFF_SIDH = 0U,
    E_CAN_BUFF_SIDL,
    E_CAN_BUFF_EID8,
    E_CAN_BUFF_EID0,
    E_CAN_BUFF_DLC,
    E_CAN_BUFF_DATA_1,
    E_CAN_BUFF_DATA_2,
    E_CAN_BUFF_DATA_3,
    E_CAN_BUFF_DATA_4,
    E_CAN_BUFF_DATA_5,
    E_CAN_BUFF_DATA_6,
    E_CAN_BUFF_DATA_7,
    E_CAN_BUFF_DATA_8,
    E_CAN_BUFF_QTY
} can_buff_t;

/* To standard CAN ID */
#define SIDH_L_SHIFT_TO_STD_CANID       ( (uint8_t)3U )
#define SIDL_R_SHIFT_TO_STD_CANID       ( (uint8_t)5U )

/* From standard CAN ID */
#define STD_CANID_MASK_TO_SIDH          ( (uint32_t)0x000007F8U )
#define STD_CANID_MASK_TO_SIDL          ( (uint32_t)0x00000007U )

#define STD_CANID_R_SHIFT_TO_SIDH       ( SIDH_L_SHIFT_IF_STD_CANID )
#define STD_CANID_L_SHIFT_TO_SIDL       ( SIDL_R_SHIFT_TO_STD_CANID )


/* To Extended CAN ID */
#define SIDL_E0_MASK_TO_STD_CANID       ( (uint8_t)0xE0U )
#define SIDL_03_MASK_TO_STD_CANID       ( (uint8_t)0x03U )

#define SIDH_L_SHIFT_TO_EXT_CANID       ( (uint8_t)21U )
#define SIDL_E0_L_SHIFT_TO_EXT_CANID    ( (uint8_t)13U )
#define SIDL_03_L_SHIFT_TO_EXT_CANID    ( (uint8_t)16U )
#define EID8_L_SHIFT_TO_EXT_CANID       ( (uint8_t) 8U )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static uint32_t build_std_canid( const uint8_t sidh, const uint8_t sidl );
static uint32_t build_ext_canid( const uint8_t sidh, const uint8_t sidl, const uint8_t eid8, const uint8_t eid0 );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */

void get_rx_buff( const en_can_rx can_rx, size_t len, uint8_t *p_buff )
{
    uint8_t reg_addr;

    switch( can_rx )
    {
    case E_CAN_RX_1:
        reg_addr = REG_RXB0SIDH;
        break;
    case E_CAN_RX_2:
        reg_addr = REG_RXB1SIDH;
        break;
    default:
        // Do nothing
        break;
    };

    if( ( NULL!= p_buff ) && ( E_CAN_BUFF_QTY == len ) )
    {
        read_reg_array( reg_addr, len, p_buff );
    }
}

void mcp2515_get_can_msg( const en_can_rx can_rx, st_can_msg *p_can_msg )
{
    uint8_t rx_buff[ E_CAN_BUFF_QTY ] = { 0U };

    uint32_t can_id;
    en_can_kind can_kind;
    uint8_t can_dlc;
    uint8_t can_data[ E_CAN_DATA_QTY ] = { 0 };

    // Get DLC
    p_can_msg->dlc = rx_buff[ E_CAN_BUFF_DLC ] & MASKOF_DLC;

    // Get Data
    if( ( 0U < p_can_msg->dlc ) && ( E_CAN_DATA_QTY >= p_can_msg->dlc ) )
    {
        memcpy( p_can_msg->data, &rx_buff[ E_CAN_BUFF_DATA_1 ], can_dlc );
    }

    // Is Standard? Extended?
    bool is_std;
    uint8_t sidl_ide;
    sidl_ide = (uint8_t)( rx_buff[ E_CAN_BUFF_SIDL ] & MASKOF_SIDL_IDE );
    is_std = ( REG_VAL_SIDL_IDE_STD != sidl_ide ) ? true : false;
    if( true == is_std )
    {
        p_can_msg->kind = E_CAN_KIND_STD;
    }
    else
    {
        p_can_msg->kind = E_CAN_KIND_EXT;
    }


    if( true == is_std )
    {
        p_can_msg->id = build_std_canid(
            rx_buff[ E_CAN_BUFF_SIDH ],
            rx_buff[ E_CAN_BUFF_SIDL ]
        );
    }
    else
    {
        p_can_msg->id = build_ext_canid(
            rx_buff[ E_CAN_BUFF_SIDH ],
            rx_buff[ E_CAN_BUFF_SIDL ],
            rx_buff[ E_CAN_BUFF_EID8 ],
            rx_buff[ E_CAN_BUFF_EID0 ]
        );
    }
}

static uint32_t build_std_canid( const uint8_t sidh, const uint8_t sidl )
{
    uint32_t can_id;

    can_id  = (uint32_t)(  (uint8_t)sidl >> SIDL_R_SHIFT_TO_STD_CANID );
    can_id |= (uint32_t)( (uint16_t)sidh << SIDH_L_SHIFT_TO_STD_CANID );

    return can_id;
}

static uint32_t build_ext_canid( const uint8_t sidh, const uint8_t sidl, const uint8_t eid8, const uint8_t eid0 )
{
    uint32_t can_id;
    uint32_t sidl_e0;
    uint32_t sidl_03;

    sidl_e0 = (uint32_t)( sidl & SIDL_E0_MASK_TO_STD_CANID );
    sidl_03 = (uint32_t)( sidl & SIDL_03_MASK_TO_STD_CANID );

    can_id  = (uint32_t)eid0;
    can_id |= (uint32_t)( (uint32_t)   eid8 <<    EID8_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)(           sidl_03 << SIDL_03_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)(           sidl_e0 << SIDL_E0_L_SHIFT_TO_EXT_CANID );
    can_id |= (uint32_t)( (uint32_t)   sidh <<    SIDH_L_SHIFT_TO_EXT_CANID );

    return can_id;
}

// static void set_can_id( const can_kind_t kind, const uint32_t can_id,
//     uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

//     /* Fails if illegal arguments. */
//     if ( is_invalid_canid_kind( kind ) || ( NULL == hdr ) )
//         return CD_FAILURE;

//     if ( CD_CANID_KIND_STD == kind ) {
//         /*--------------------------*/
//         /* Case of standard format. */
//         /*--------------------------*/

//         /* Fails if too large. */
//         if ( CD_MAXOF_STD_CANID < can_id )
//             return CD_FAILURE;

//         hdr[ MCP2515_CANHDR_EID0 ] = 0U;
//         hdr[ MCP2515_CANHDR_EID8 ] = 0U;
//         hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( can_id << 5U ) & 0xE0U );
//         hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 3U );
//     } 
//     else {
//         /*--------------------------*/
//         /* Case of extended format. */
//         /*--------------------------*/

//         /* Fails if too large. */
//         if ( CD_MAXOF_EXT_CANID < can_id )
//             return CD_FAILURE;

//         hdr[ MCP2515_CANHDR_EID0 ] = (uint8_t)can_id;
//         hdr[ MCP2515_CANHDR_EID8 ] = (uint8_t)( can_id >> 8U );
//         hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( ( can_id >> 16U ) & 0x03U )
//             | MASKOF_SIDL_IDE | ( ( can_id >> 13U ) & 0xE0U ) );
//         hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 21U );
//     }
// }

// static cd_result_t set_can_dlc( const uint8_t dlc, const bool is_remote,
//     uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

//     uint8_t reg_val = 0U;

//     /* Fails if illegal arguments. */
//     if ( ( CD_CANDLC_MAXOF_LEN < dlc ) || ( NULL == hdr ) )
//         return CD_FAILURE;
    
//     reg_val = (uint8_t)( reg_val | (uint8_t)( dlc & 0xF ) );

//     if( true == is_remote ) {

//         reg_val = (uint8_t)( reg_val | MASKOF_RTR );
//     }

//     hdr[ MCP2515_CANHDR_DLC ] = reg_val;

//     return CD_SUCCESS;
// }

