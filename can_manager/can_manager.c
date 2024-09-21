/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>

/* My standard library */
#include <my_error_code.h>
#include <my_task_cfg.h>

/* Driver */
#include <hardware_driver.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum en_cam_event
{
    E_CAM_EVT_RESET     = 0x000001U,
    E_CAM_EVT_RECV_RX1  = 0x000002U,
    E_CAM_EVT_RECV_RX2  = 0x000004U,
    E_CAM_EVT_ALL       = (uint)(
        E_CAM_EVT_RESET | E_CAM_EVT_RECV_RX1 | E_CAM_EVT_RECV_RX2
    )
} cam_event_t;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void task( void *nouse );
static void irq_handler( const uint8_t fact );
static void reset_controller( void );
static void proc_recv_can( const en_can_rx_t rx );

/* -------------------------------------------------------------------------- */
/* Global                                                                  */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_ret_t cam_create_task( void )
{
    BaseType_t ret = pdFAIL;

    g_evt_hndl = xEventGroupCreate();

    if( NULL != g_evt_hndl )
    {
        ret = xTaskCreate( task, "CAN_MANAGER", 1024, NULL, E_TASK_PRIO_CAN, &g_tsk_hndl );
    }

    return ( pdPASS == ret ) ? E_OK : E_NOK;
}

/* -------------------------------------------------------------------------- */
/* Private function                                                           */
/* -------------------------------------------------------------------------- */
static void task( void *nouse )
{
    EventBits_t events;

    st_can_msg_t can_msg = { 0U, E_CAN_KIND_STD, 0U, { 0U } };

    while( true )
    {
        events = xEventGroupWaitBits( g_evt_hndl, E_CAM_EVT_ALL, pdTRUE, pdFALSE, portMAX_DELAY );

        if( (EventBits_t)E_CAM_EVT_RESET != ( (EventBits_t)E_CAM_EVT_RESET & events ) )
        {
            /* Reset CAN controller */
            reset_controller();
        }
        else
        {
            if( (EventBits_t)E_CAM_EVT_RECV_RX1 != ( (EventBits_t)E_CAM_EVT_RECV_RX1 & events ) )
            {
                /* Read CAN message from RX1 */
                hwd_get_can_msg( E_CAN_RX1, &can_msg );

                /* Process a received CAN message */
                proc_recv_can( E_CAN_RX1 );

                /* Enable CAN IRQ factor of the RX1 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_RX1 );
            }

            if( (EventBits_t)E_CAM_EVT_RECV_RX2 != ( (EventBits_t)E_CAM_EVT_RECV_RX2 & events ) )
            {
                /* Read CAN message from RX2 */
                hwd_get_can_msg( E_CAN_RX2, &can_msg );

                /* Process a received CAN message */
                proc_recv_can( E_CAN_RX2 );

                /* Enable CAN IRQ factor of the RX2 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_RX2 );
            }
        }
    }

}

static void irq_handler( const uint8_t fact )
{
    BaseType_t hptw = pdFALSE;

    if( (uint8_t)E_CAN_IRQ_FACT_RX1 == (uint8_t)( (uint8_t)E_CAN_IRQ_FACT_RX1 & fact ) )
    {
        /* Cause event CAN received by RX1 */
        xEventGroupSetBitsFromISR( g_evt_hndl, E_CAM_EVT_RECV_RX1, &hptw );
    }

    if( (uint8_t)E_CAN_IRQ_FACT_RX1 == (uint8_t)( (uint8_t)E_CAN_IRQ_FACT_RX2 & fact ) )
    {
        /* Cause event CAN received by RX2 */
        xEventGroupSetBitsFromISR( g_evt_hndl, E_CAM_EVT_RECV_RX2, &hptw );
    }

    /* Exit to context switch if necessary */
    portYIELD_FROM_ISR( hptw );
}

static void reset_controller( void )
{
    /* Reset CAN controller */
    hwd_reset_can_controller();

    /* Set IRQ handler */
    hwd_set_can_irq_callback( irq_handler );

    /* Enable CAN IRQ */
    hwd_enable_can_irq( true );

    /* Begin CAN communication */
    hwd_begin_can_communication();
}

static void proc_recv_can( const en_can_rx_t rx )
{
    typedef void ( *can_recv )( const st_can_msg_t *p_can_msg );

    typedef struct st_can_proc
    {
        uint32_t id;
        can_recv recv_func;
    } can_proc_t;

    const can_proc_t proc_tbl[] = {
        { 0x477U, NULL }
    };

    st_can_msg_t can_msg = { 0U, E_CAN_KIND_STD, 0U, { 0U } };

    uint8_t idx;
    uint8_t tbl_qty;

    hwd_get_can_msg( rx, &can_msg );

    tbl_qty = sizeof( proc_tbl ) / sizeof( can_proc_t );

    for( idx = 0U; idx < tbl_qty; idx++ )
    {
        if( ( can_msg.id == proc_tbl[idx].id ) && ( NULL != proc_tbl[idx].recv_func ) )
        {
            proc_tbl[idx].recv_func( &can_msg );
        }
    }
}