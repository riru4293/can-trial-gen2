#ifndef MCP2515_H
#define MCP2515_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <cdf_err.h>
#include <hwd_api.h>

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
void mcp2515_reset( void );
void mcp2515_start_can_comm( void );
void mcp2515_set_can_irq_cbk( const fn_hwd_can_irq_cbk cbk );
void mcp2515_enable_can_irq_fact( const uint8_t fact );

en_cdf_err mcp2515_get_can_msg( const en_hwd_can_rx can_rx, st_cdf_can_msg *p_can_msg );
en_cdf_err mcp2515_set_can_msg( const en_hwd_can_tx can_tx, const st_cdf_can_msg *p_can_msg );

#endif /* MCP2515_H */
