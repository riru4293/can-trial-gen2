#ifndef MCP2515_H
#define MCP2515_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <hardware_driver.h>

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
void mcp2515_reset( void );
void mcp2515_begin_communication( void );
void mcp2515_get_can_msg( const en_can_rx rx, st_can_msg *p_can_msg );
void mcp2515_set_can_irq_cbk( const fn_can_irq_cbk cbk );
void mcp2515_enable_can_irq_fact( const uint8_t fact );

#endif /* MCP2515_H */
