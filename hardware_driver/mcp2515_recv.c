/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

/* My standard library */
#include <cdf_err.h>

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

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void read_rx_buff( const en_hwd_can_rx can_rx, const size_t len, uint8_t *p_buff );
static en_cdf_can_kind resolve_can_kind( const size_t len, const uint8_t *p_buff );
static uint32_t resolve_canid( const en_cdf_can_kind kind, const size_t len, const uint8_t *p_buff );
static uint32_t resolve_std_canid( const size_t len, const uint8_t *p_buff );
static uint32_t resolve_ext_canid( const size_t len, const uint8_t *p_buff );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
void mcp2515_get_can_msg( const en_hwd_can_rx can_rx, st_cdf_can_msg *p_can_msg )
{
    uint8_t *p_dlc;
    uint8_t buff[ E_CAN_BUFF_QTY ] = { 0U };

    if( NULL != p_can_msg )
    {
        /* Read RX buffer */
        read_rx_buff( can_rx, sizeof( buff ), buff );

        /* Resolve CAN kind */
        p_can_msg->kind = resolve_can_kind( sizeof( buff ), buff );

        /* Resolve CAN id */
        p_can_msg->id = resolve_canid( p_can_msg->kind, sizeof( buff ), buff );

        /* Resolve DLC */
        p_dlc = &( p_can_msg->dlc );
        *p_dlc = (uint8_t)( buff[ E_CAN_BUFF_HDR_5 ] & MASKOF_DLC );
        
        /* Get Data */
        if( ( E_CDF_CAN_DLC_MIN < *p_dlc ) && ( E_CDF_CAN_DLC_MAX >= *p_dlc ) )
        {
            memcpy( p_can_msg->data, &buff[ E_CAN_BUFF_DATA_1 ], *p_dlc );
        }
    }
}

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void read_rx_buff( const en_hwd_can_rx can_rx, const size_t len, uint8_t *p_buff )
{
    switch ( can_rx )
    {
    case E_HWD_CAN_RX_1:
        mcp2515_read_rx1( len, p_buff );
        break;
    
    case E_HWD_CAN_RX_2:
        mcp2515_read_rx2( len, p_buff );
        break;
    
    default:
        /* Do nothing */
        break;
    }
}

static en_cdf_can_kind resolve_can_kind( const size_t len, const uint8_t *p_buff )
{
    const uint8_t C_STD = 0x00U;
    en_cdf_can_kind kind = E_CDF_CAN_KIND_INVALID;

    if( ( E_CAN_BUFF_QTY == len ) && ( NULL != p_buff ) )
    {
        if ( C_STD == (uint8_t)( p_buff[ E_CAN_BUFF_HDR_2 ] & MASKOF_SIDL_IDE ) )
        {
            kind = E_CDF_CAN_KIND_STD;
        }
        else
        {
            kind = E_CDF_CAN_KIND_EXT;
        }
    }

    return kind;
}

static uint32_t resolve_canid( const en_cdf_can_kind kind, const size_t len, const uint8_t *p_buff )
{
    uint32_t id;

    switch ( kind )
    {
    case E_CDF_CAN_KIND_STD:
        id = resolve_std_canid( len, p_buff );
        break;
    
    case E_CDF_CAN_KIND_EXT:
        id = resolve_ext_canid( len, p_buff );
        break;
    
    default:
        id = CDF_CAN_ID_INVALID;
        break;
    }

    return id;
}

static uint32_t resolve_std_canid( const size_t len, const uint8_t *p_buff )
{
    /* ---------------------------------------------- */
    /* CAN_ID layout                                   */
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

    uint32_t can_id = CDF_CAN_ID_INVALID;

    if( ( E_CAN_BUFF_QTY == len ) && ( NULL != p_buff ) )
    {
        can_id  = ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_2 ] >> C_HDR2_BIT_OFFSET );
        can_id |= ( (uint32_t)p_buff[ E_CAN_BUFF_HDR_1 ] << C_HDR1_BIT_OFFSET );
    }

    return can_id;
}

static uint32_t resolve_ext_canid( const size_t len, const uint8_t *p_buff )
{
    /* ---------------------------------------------- */
    /* CAN_ID layout                                   */
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

    uint32_t can_id = CDF_CAN_ID_INVALID;

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
