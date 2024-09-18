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

/* -------------------------------------------------------------------------- */
/* Global                                                                  */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
ret_cd_t cam_create_task( void )
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

    while( true )
    {
        events = xEventGroupWaitBits( g_evt_hndl, E_CAM_EVT_ALL, pdTRUE, pdFALSE, portMAX_DELAY );

        if( E_CAM_EVT_RESET != ( E_CAM_EVT_RESET & events ) )
        {
            /* Reset CAN controller */
            reset_controller();
        }
        else
        {
            if( E_CAM_EVT_RECV_RX1 != ( E_CAM_EVT_RECV_RX1 & events ) )
            {
                // RX1割込要因を有効化

            }
            if( E_CAM_EVT_RECV_RX2 != ( E_CAM_EVT_RECV_RX2 & events ) )
            {
                // RX2割込要因を有効化
            }
            // if( E_CAM_EVT_NONE != ( E_CAM_EVT_CAN_IRQ & events ) )
            // {

            //     /* Get occurred CAN IRQ */
            //     occurred_irq = hwd_get_occurred_irq();

            //     /* If an IRQ occurred, disable it factor. */
            //     /* Because it prevents reentrancy by the same IRQ factor. */
            //     if ( DRV_IRQ_NONE != occurred_irq )
            //     {
            //         hwd_enable_can_ctrl_irq( occurred_irq );

            //         hwd_enable_can_irq_fact( occurred_irq );
            //         hwd_disable_can_irq_fact( occurred_irq );

            //         // CAN_IRQ要因をHWDRVで定義（実体はMCP2515の値）
            //     }

            //     /* Get CAN frame from RX1 */

            //     /* Enable CAN IRQ */
            //     hwd_enable_can_ctrl_irq( true );
            // }
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