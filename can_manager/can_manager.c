/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

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
static void proc_recv_can( const en_can_rx can_rx );

/* -------------------------------------------------------------------------- */
/* Global                                                                  */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_err cam_create_task( void )
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

    reset_controller();

    while( true )
    {
        events = xEventGroupWaitBits( g_evt_hndl, E_CAM_EVT_ALL, pdTRUE, pdFALSE, portMAX_DELAY );

        if( (EventBits_t)E_CAM_EVT_RESET == ( (EventBits_t)E_CAM_EVT_RESET & events ) )
        {
            /* Reset CAN controller */
            reset_controller();
        }
        else
        {
            if( (EventBits_t)E_CAM_EVT_RECV_RX1 == ( (EventBits_t)E_CAM_EVT_RECV_RX1 & events ) )
            {
                /* Process a received CAN message */
                proc_recv_can( E_CAN_RX_1 );

                /* Enable CAN IRQ factor of the RX1 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_RX1 );
            }

            if( (EventBits_t)E_CAM_EVT_RECV_RX2 == ( (EventBits_t)E_CAM_EVT_RECV_RX2 & events ) )
            {
                /* Process a received CAN message */
                proc_recv_can( E_CAN_RX_2 );

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
    hwd_reset_can_ctrl();

    /* Set IRQ handler */
    hwd_set_can_irq_cbk( irq_handler );

    /* Enable CAN IRQ factor */
    hwd_enable_can_irq_fact( E_CAN_IRQ_FACT_RX1 | E_CAN_IRQ_FACT_RX2 );

    /* Enable CAN IRQ */
    hwd_enable_can_irq( true );

    /* Start CAN communication */
    hwd_start_can_comm();
}

static void proc_recv_can( const en_can_rx can_rx )
{
    st_can_msg can_msg = { CANID_INVALID, E_CAN_KIND_INVALID, E_CAN_DLC_MIN, { 0U } };

    switch ( can_rx )
    {
    case E_CAN_RX_1:
    case E_CAN_RX_2:
        hwd_get_can_msg( can_rx, &can_msg );

        printf("ID: %00X, DLC: %d, DATA: %0X %0X %0X %0X %0X %0X %0X %0X\n"
            , can_msg.id
            , can_msg.dlc
            , can_msg.data[E_CAN_DATA_1]
            , can_msg.data[E_CAN_DATA_2]
            , can_msg.data[E_CAN_DATA_3]
            , can_msg.data[E_CAN_DATA_4]
            , can_msg.data[E_CAN_DATA_5]
            , can_msg.data[E_CAN_DATA_6]
            , can_msg.data[E_CAN_DATA_7]
            , can_msg.data[E_CAN_DATA_8]
        );
        break;
    
    default:
        /* Do nothing */
        break;
    }
}