/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>

#include <app_errno.h>
#include <hwd_api.h>
#include <task_cfg.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_NTM_EVT_RESET     = 0x000001U,
    E_NTM_EVT_RECV_RX1  = 0x000002U,
    E_NTM_EVT_RECV_RX2  = 0x000004U,
    E_NTM_EVT_RECV_TX1  = 0x000008U,
    E_NTM_EVT_RECV_TX2  = 0x000010U,
    E_NTM_EVT_RECV_TX3  = 0x000020U,
    E_NTM_EVT_ALL       = (uint32_t)(
        E_NTM_EVT_RESET | E_NTM_EVT_RECV_RX1 | E_NTM_EVT_RECV_RX2
        | E_NTM_EVT_RECV_TX1 | E_NTM_EVT_RECV_TX2 | E_NTM_EVT_RECV_TX3
    )
} en_ntm_event;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void task( void *nouse );
static void irq_handler( const uint8_t fact );
static void reset_controller( void );
static void proc_recv_can( const en_hwd_can_rx can_rx );

/* -------------------------------------------------------------------------- */
/* Global                                                                  */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno ntm_create_task( void )
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
        events = xEventGroupWaitBits( g_evt_hndl, E_NTM_EVT_ALL, pdTRUE, pdFALSE, portMAX_DELAY );

        if( (EventBits_t)E_NTM_EVT_RESET == ( (EventBits_t)E_NTM_EVT_RESET & events ) )
        {
            /* Reset CAN controller */
            reset_controller();
        }
        else
        {
            if( (EventBits_t)E_NTM_EVT_RECV_RX1 == ( (EventBits_t)E_NTM_EVT_RECV_RX1 & events ) )
            {
                /* Process a received CAN message */
                proc_recv_can( E_HWD_CAN_RX_1 );

                /* Enable CAN IRQ factor of the RX1 */
                hwd_enable_can_irq_fact( (uint8_t)E_HWD_CAN_IRQ_FACT_RX1 );
            }

            if( (EventBits_t)E_NTM_EVT_RECV_RX2 == ( (EventBits_t)E_NTM_EVT_RECV_RX2 & events ) )
            {
                /* Process a received CAN message */
                proc_recv_can( E_HWD_CAN_RX_2 );

                /* Enable CAN IRQ factor of the RX2 */
                hwd_enable_can_irq_fact( (uint8_t)E_HWD_CAN_IRQ_FACT_RX2 );
            }
        }
    }

}

static void irq_handler( const uint8_t fact )
{
    typedef struct
    {
        en_hwd_can_irq_fact fact;
        en_ntm_event evt;
    } st_fact_to_evt;

    const st_fact_to_evt T_CONV[] =
    {
        { E_HWD_CAN_IRQ_FACT_RX1, E_NTM_EVT_RECV_RX1 },
        { E_HWD_CAN_IRQ_FACT_RX2, E_NTM_EVT_RECV_RX2 },
        { E_HWD_CAN_IRQ_FACT_TX1, E_NTM_EVT_RECV_TX1 },
        { E_HWD_CAN_IRQ_FACT_TX2, E_NTM_EVT_RECV_TX2 },
        { E_HWD_CAN_IRQ_FACT_TX3, E_NTM_EVT_RECV_TX3 }
    };

    uint8_t idx;
    BaseType_t hptw = pdFALSE;

    for( idx = 0U; idx < sizeof( T_CONV ); idx++ )
    {
        if( T_CONV[ idx ].fact == (uint8_t)T_CONV[ idx ].fact & fact )
        {
            /* Cause an event per factor */
            xEventGroupSetBitsFromISR( g_evt_hndl, T_CONV[ idx ].evt, &hptw );
        }
    }

    /* Exit to context switch if necessary */
    portYIELD_FROM_ISR( hptw );
}

static void reset_controller( void )
{
    const uint8_t C_FACT = (E_HWD_CAN_IRQ_FACT_RX1 | E_HWD_CAN_IRQ_FACT_RX2 |
        E_HWD_CAN_IRQ_FACT_TX1 | E_HWD_CAN_IRQ_FACT_TX2 | E_HWD_CAN_IRQ_FACT_TX3 );

    /* Reset CAN controller */
    hwd_reset_can_ctrl();

    /* Set IRQ handler */
    hwd_set_can_irq_cbk( irq_handler );

    /* Enable CAN IRQ factor */
    hwd_enable_can_irq_fact( C_FACT );

    /* Enable CAN IRQ */
    hwd_enable_irq_handling( true );

    /* Start CAN communication */
    hwd_start_can_comm();
}

static void proc_recv_can( const en_hwd_can_rx can_rx )
{
    st_can_msg can_msg = { CAN_ID_INVALID, E_CAN_KIND_INVALID, E_CAN_DLC_MIN, { 0U } };

    switch ( can_rx )
    {
    case E_HWD_CAN_RX_1:
    case E_HWD_CAN_RX_2:
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