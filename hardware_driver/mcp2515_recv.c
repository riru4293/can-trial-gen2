/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <string.h>

#include <app_errno.h>
#include <private/mcp2515.h>
#include <private/mcp2515_spi_cmd.h>
#include <private/mcp2515_reg.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void read_rx_buff( const en_can_rx can_rx, const size_t n, uint8_t buff[n] );
static en_can_kind resolve_can_kind( const size_t n, const uint8_t buff[n] );
static uint32_t resolve_can_id( const en_can_kind kind, const size_t n, const uint8_t buff[n] );
static uint32_t resolve_std_can_id( const size_t n, const uint8_t buff[n] );
static uint32_t resolve_ext_can_id( const size_t n, const uint8_t buff[n] );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno mcp2515_get_can_msg( const en_can_rx can_rx, st_can_msg *p_can_msg )
{
    en_errno result = E_NOK;
    uint8_t buff[ E_CAN_BUFF_QTY ] = { 0U };
    uint8_t *p_dlc;

    if( NULL != p_can_msg )
    {
        /* Read RX buffer */
        read_rx_buff( can_rx, sizeof( buff ), buff );

        /* Resolve CAN kind */
        p_can_msg->kind = resolve_can_kind( sizeof( buff ), buff );

        /* Resolve CAN id */
        p_can_msg->id = resolve_can_id( p_can_msg->kind, sizeof( buff ), buff );

        /* Resolve DLC */
        p_dlc = &( p_can_msg->dlc );
        *p_dlc = ( buff[ E_CAN_BUFF_HDR_5 ] & REG_MASK_DLC_DLC );
        
        /* Get Data */
        if( ( E_CAN_DLC_MIN < *p_dlc ) && ( E_CAN_DLC_MAX >= *p_dlc ) )
        {
            memcpy( p_can_msg->data, &buff[ E_CAN_BUFF_DATA_1 ], *p_dlc );
        }

        result = E_OK;
    }

    return result;
}

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void read_rx_buff( const en_can_rx can_rx, const size_t n, uint8_t buff[n] )
{
    uint8_t cmd = SPI_CMD_INVALID;

    /* Resolve the read command per RX */
    switch ( can_rx )
    {
    case E_CAN_RX_1:
        cmd = SPI_CMD_READ_RX_1;
        break;
    
    case E_CAN_RX_2:
        cmd = SPI_CMD_READ_RX_2;
        break;
    
    default:
        /* Do nothing */
        break;
    }

    if( ( cmd != SPI_CMD_INVALID ) && ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        /* Begin SPI communication */
        mcp2515_begin_spi();

        /* Read RX buffer */
        mcp2515_write_spi( cmd );
        mcp2515_read_spi_array( n, buff );

        /* End SPI communication */
        mcp2515_end_spi();
    }
}

static en_can_kind resolve_can_kind( const size_t n, const uint8_t buff[n] )
{
    uint8_t reg_val;
    en_can_kind kind;

    if( ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        /* Read the IDE of register SIDL */
        reg_val = ( buff[ E_CAN_BUFF_HDR_2 ] & REG_MASK_SIDL_IDE );

        /* Resolve the CAN message kind */
        switch ( reg_val )
        {
        case REG_VAL_CAN_KIND_STD:
            kind = E_CAN_KIND_STD;
            break;
        
        case REG_VAL_CAN_KIND_EXT:
            kind = E_CAN_KIND_EXT;
            break;
        
        default:
            /* Unreachable */
            kind = E_CAN_KIND_INVALID;
            break;
        }
    }

    return kind;
}

static uint32_t resolve_can_id( const en_can_kind kind, const size_t n, const uint8_t buff[n] )
{
    uint32_t id;

    switch ( kind )
    {
    case E_CAN_KIND_STD:
        id = resolve_std_can_id( n, buff );
        break;
    
    case E_CAN_KIND_EXT:
        id = resolve_ext_can_id( n, buff );
        break;
    
    default:
        id = CAN_ID_INVALID;
        break;
    }

    return id;
}

static uint32_t resolve_std_can_id( const size_t n, const uint8_t buff[n] )
{
    /* ---------------------------------------------- */
    /* CAN_ID layout                                  */
    /*                                                */
    /*       bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0  */
    /*      +----+----+----+----+----+----+----+----+ */
    /* HDR1 |ID11|ID10|ID9 |ID8 |ID7 |ID6 |ID5 |ID4 | */
    /* HDR2 |ID3 |ID2 |ID1 |-   |-   |-   |-   |-   | */
    /*      +----+----+----+----+----+----+----+----+ */
    /*                                                */
    /* ---------------------------------------------- */

    const uint8_t C_HDR1_OFFSET = 3U;
    const uint8_t C_HDR2_OFFSET = 5U;

    uint32_t can_id = CAN_ID_INVALID;

    if( ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        can_id  = ( (uint32_t)buff[ E_CAN_BUFF_HDR_2 ] >> C_HDR2_OFFSET );
        can_id |= ( (uint32_t)buff[ E_CAN_BUFF_HDR_1 ] << C_HDR1_OFFSET );
    }

    return can_id;
}

static uint32_t resolve_ext_can_id( const size_t n, const uint8_t buff[n] )
{
    /* ---------------------------------------------- */
    /* CAN_ID layout                                  */
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

    const uint8_t C_HDR1_OFFSET     = 21U;
    const uint8_t C_HDR2_EID_OFFSET = 16U;  /* Range of 0x03 in HDR2(SIDL) */
    const uint8_t C_HDR2_SID_OFFSET = 13U;  /* Range of 0xE0 in HDR2(SIDL) */
    const uint8_t C_HDR3_OFFSET     =  8U;

    uint32_t can_id = CAN_ID_INVALID;

    if( ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        can_id  = buff[ E_CAN_BUFF_HDR_4 ];
        can_id |= ( (uint32_t)buff[ E_CAN_BUFF_HDR_3 ] << C_HDR3_OFFSET );
        can_id |= ( (uint32_t)( buff[ E_CAN_BUFF_HDR_2 ] & REG_MASK_SIDL_EID ) << C_HDR2_EID_OFFSET );
        can_id |= ( (uint32_t)( buff[ E_CAN_BUFF_HDR_2 ] & REG_MASK_SIDL_SID ) << C_HDR2_SID_OFFSET );
        can_id |= ( (uint32_t)buff[ E_CAN_BUFF_HDR_1 ] << C_HDR1_OFFSET );
    }

    return can_id;
}
