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
static void set_std_can_id( const uint32_t can_id, const size_t n, uint8_t buff[n] );
static void set_ext_can_id( const uint32_t can_id, const size_t n, uint8_t buff[n] );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno mcp2515_set_can_msg( const en_can_tx can_tx, const st_can_msg *p_can_msg )
{
    en_errno result = E_NOK;
    uint8_t buff[ E_CAN_BUFF_QTY ] = { 0U };
    en_can_kind kind;
    uint32_t id;
    uint8_t dlc;

    if( NULL != p_can_msg )
    {
        id = p_can_msg->id;
        kind = p_can_msg->kind;

        switch ( kind )
        {
        case E_CAN_KIND_STD:
            if( STD_CAN_ID_MAX >= id )
            {
                /* Set CAN message kind */
                buff[ E_CAN_BUFF_HDR_2 ] = REG_VAL_CAN_KIND_STD;

                /* Set standard CAN id */
                set_std_can_id( id, sizeof( buff ), buff );

                result = E_OK;
            }
            break;
        
        case E_CAN_KIND_EXT:
            if( EXT_CAN_ID_MAX >= id )
            {
                /* Set CAN message kind */
                buff[ E_CAN_BUFF_HDR_2 ] = REG_VAL_CAN_KIND_EXT;

                /* Set extended CAN id */
                set_ext_can_id( id, sizeof( buff ), buff );

                result = E_OK;
            }
            break;
        
        default:
            /* Do nothing */
            break;
        }

        if( E_OK == result )
        {
            dlc = p_can_msg->dlc;

            if( E_CAN_DLC_8 >= dlc )
            {
                /* Set DLC */
                buff[ E_CAN_BUFF_DATA_1 ] |= ( dlc & REG_MASK_DLC_DLC );

                /* Set Data */
                if( E_CAN_DLC_0 < dlc )
                {
                    memcpy( &buff[ E_CAN_BUFF_DATA_1 ], &p_can_msg->data, dlc );
                }
            }
            else
            {
                result = E_NOK;
            }
        }
    }

    return result;
}

/* -------------------------------------------------------------------------- */
/* Friend functions                                                           */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void set_std_can_id( const uint32_t can_id, const size_t n, uint8_t buff[n] )
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

    if( ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        buff[ E_CAN_BUFF_DATA_1 ]  = ( can_id >> C_HDR1_OFFSET );
        buff[ E_CAN_BUFF_DATA_2 ] |= ( (uint8_t)( can_id << C_HDR2_OFFSET ) & REG_MASK_SIDL_SID );
    }
}

static void set_ext_can_id( const uint32_t can_id, const size_t n, uint8_t buff[n] )
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

    if( ( E_CAN_BUFF_QTY == n ) && ( NULL != buff ) )
    {
        buff[ E_CAN_BUFF_DATA_1 ]  = ( can_id >> C_HDR1_OFFSET );
        buff[ E_CAN_BUFF_DATA_2 ] |= ( (uint8_t)( can_id >> C_HDR2_EID_OFFSET ) & REG_MASK_SIDL_EID );
        buff[ E_CAN_BUFF_DATA_2 ] |= ( (uint8_t)( can_id >> C_HDR2_SID_OFFSET ) & REG_MASK_SIDL_SID );
        buff[ E_CAN_BUFF_DATA_3 ]  = ( can_id >> C_HDR3_OFFSET );
        buff[ E_CAN_BUFF_DATA_4 ]  = can_id;
    }
}
