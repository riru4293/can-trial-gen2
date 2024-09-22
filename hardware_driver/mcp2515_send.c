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

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */

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
