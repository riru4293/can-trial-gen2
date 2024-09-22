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
typedef enum
{
    E_CAN_BUFF_HDR_1,
    E_CAN_BUFF_HDR_2,
    E_CAN_BUFF_HDR_3,
    E_CAN_BUFF_HDR_4,
    E_CAN_BUFF_HDR_5,
    E_CAN_BUFF_DATA_1,
    E_CAN_BUFF_DATA_2,
    E_CAN_BUFF_DATA_3,
    E_CAN_BUFF_DATA_4,
    E_CAN_BUFF_DATA_5,
    E_CAN_BUFF_DATA_6,
    E_CAN_BUFF_DATA_7,
    E_CAN_BUFF_DATA_8,
    E_CAN_BUFF_QTY
} en_can_buff;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static uint32_t read_std_canid( const size_t len, const uint8_t *p_buff );
static uint32_t read_ext_canid( const size_t len, const uint8_t *p_buff );

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

void read_rx_buff( const en_can_rx can_rx, const size_t len, uint8_t *p_buff )
{
    switch ( can_rx )
    {
    case E_CAN_RX_1:
        mcp2515_read_rx1( len, p_buff );
        break;
    
    case E_CAN_RX_2:
        mcp2515_read_rx2( len, p_buff );
        break;
    
    default:
        /* Do nothing */
        break;
    }
}

void mcp2515_get_can_msg( const en_can_rx can_rx, st_can_msg *p_can_msg )
{
    uint8_t rx_buff[ E_CAN_BUFF_QTY ] = { 0U };

    /* Read RX buffer */
    read_rx_buff( can_rx, sizeof( rx_buff ), rx_buff );

    uint32_t can_id;
    en_can_kind can_kind;
    uint8_t can_dlc;
    uint8_t can_data[ E_CAN_DATA_QTY ] = { 0 };


    // Get DLC
    p_can_msg->dlc = rx_buff[ E_CAN_BUFF_HDR_5 ] & MASKOF_DLC;

    // Get Data
    if( ( 0U < p_can_msg->dlc ) && ( E_CAN_DATA_QTY >= p_can_msg->dlc ) )
    {
        memcpy( p_can_msg->data, &rx_buff[ E_CAN_BUFF_DATA_1 ], can_dlc );
    }

    // Is Standard? Extended?
    bool is_std;
    uint8_t sidl_ide;
    sidl_ide = (uint8_t)( rx_buff[ E_CAN_BUFF_HDR_2 ] & MASKOF_SIDL_IDE );
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
        p_can_msg->id = read_ext_canid( sizeof( rx_buff ), rx_buff );
    }
    else
    {
        p_can_msg->id = read_ext_canid( sizeof( rx_buff ), rx_buff );
    }
}

static uint32_t read_std_canid( const size_t len, const uint8_t *p_buff )
{
    /* ---------------------------------------------- */
    /* CANID layout                                   */
    /*                                                */
    /*       bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0  */
    /*      +----+----+----+----+----+----+----+----+ */
    /* HDR1 |ID11|ID10|ID9 |ID8 |ID7 |ID6 |ID5 |ID4 | */
    /* HDR2 |ID3 |ID2 |ID1 |-   |-   |-   |-   |-   | */
    /*      +----+----+----+----+----+----+----+----+ */
    /*                                                */
    /* ---------------------------------------------- */

    const uint8_t C_HDR1_BIT_OFFSET = 3U;
    const uint8_t C_HDR2_BIT_OFFSET = 5U;

    uint32_t can_id = CANID_INVALID;

    if( ( E_CAN_BUFF_QTY == len ) && ( NULL != p_buff ) )
    {
        can_id  = ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_2 ] >> C_HDR2_BIT_OFFSET );
        can_id |= ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_1 ] << C_HDR1_BIT_OFFSET );
    }

    return can_id;
}

static uint32_t read_ext_canid( const size_t len, const uint8_t *p_buff )
{
    /* ---------------------------------------------- */
    /* CANID layout                                   */
    /*                                                */
    /*       bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0  */
    /*      +----+----+----+----+----+----+----+----+ */
    /* HDR1 |ID29|ID28|ID27|ID26|ID25|ID24|ID23|ID22| */
    /* HDR2 |ID21|ID20|ID19|-   |-   |-   |ID18|ID17| */
    /* HDR3 |ID16|ID15|ID14|ID13|ID12|ID11|ID10|ID9 | */
    /* HDR4 |ID8 |ID7 |ID6 |ID5 |ID4 |ID3 |ID2 |ID1 | */
    /*      +----+----+----+----+----+----+----+----+ */
    /*                                                */
    /* ---------------------------------------------- */

    const uint8_t C_HDR2_BIT0_2_MASK = 0x03U;
    const uint8_t C_HDR2_BIT5_7_MASK = 0xE0U;

    const uint8_t C_HDR1_BIT_OFFSET    = 21U;
    const uint8_t C_HDR2_BIT0_2_OFFSET = 16U;
    const uint8_t C_HDR2_BIT5_7_OFFSET = 13U;
    const uint8_t C_HDR3_BIT_OFFSET    =  8U;

    uint32_t can_id = CANID_INVALID;

    if( ( E_CAN_BUFF_QTY == len ) && ( NULL != p_buff ) )
    {
        can_id  = (uint32_t)p_buff[ E_CAN_BUFF_HDR_4 ];
        can_id |= ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_3 ] << C_HDR3_BIT_OFFSET );
        can_id |= ( (uint32_t)( p_buff[ E_CAN_BUFF_HDR_2 ] & C_HDR2_BIT0_2_MASK ) << C_HDR2_BIT0_2_OFFSET );
        can_id |= ( (uint32_t)( p_buff[ E_CAN_BUFF_HDR_2 ] & C_HDR2_BIT5_7_MASK ) << C_HDR2_BIT5_7_OFFSET );
        can_id |= ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_1 ] << C_HDR1_BIT_OFFSET );
    }

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

