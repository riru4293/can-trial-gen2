/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <timers.h>
#include <queue.h>

#include <app_errno.h>
#include <hwd_api.h>
#include <task_cfg.h>
#include <can.h>
#include <private/can_id.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define DELIVERY_PERIOD_MS              ( (uint8_t)10U )
#define DELIVERY_PERIOD                 ( pdMS_TO_TICKS( DELIVERY_PERIOD_MS ) )
#define SEND_QUEUE_WAIT_MS              ( (uint8_t)0U )
#define SEND_QUEUE_ELEM_MAX             ( (uint8_t)10U )
#define SEND_QUEUE_ELEM_SIZE            ( sizeof( EXT_CAN_ID_MAX ) )
#define NO_PERIODIC                     ( (uint8_t)0U )
#define NO_REMAINING                    ( (uint8_t)0U )

#define E_CAN_NNN_INIT                  ( ( uint8_t[ E_CAN_DLC_MAX ] ){ 0xFFU, 0x05U, 0xA0U, 0x07U, 0x00U, 0x00U, 0x00U, 0x00U } )
#define E_CAN_YYY_INIT                  ( ( uint8_t[ E_CAN_DLC_MAX ] ){ 0xFFU, 0x1FU, 0x1FU, 0x77U, 0x44U, 0x33U, 0x22U, 0x55U } )

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
    E_NTM_EVT_RESET     = 0x000001U,
    E_NTM_EVT_RECV_RX1  = 0x000002U,
    E_NTM_EVT_RECV_RX2  = 0x000004U,
    E_NTM_EVT_SENT_TX1  = 0x000008U,
    E_NTM_EVT_SENT_TX2  = 0x000010U,
    E_NTM_EVT_SENT_TX3  = 0x000020U,
    E_NTM_EVT_ALL       = (uint32_t)(
        E_NTM_EVT_RESET | E_NTM_EVT_RECV_RX1 | E_NTM_EVT_RECV_RX2
        | E_NTM_EVT_SENT_TX1 | E_NTM_EVT_SENT_TX2 | E_NTM_EVT_SENT_TX3
    )
} en_ntm_event;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void task( void *nouse );
static void irq_handler( const uint8_t fact );
static void reset_controller( void );
static void proc_recv_can( const en_can_rx can_rx );
static void delivery_cbk( const TimerHandle_t hndl );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;
static TimerHandle_t g_delivery_timer_hndl = NULL;
static volatile QueueHandle_t g_send_queue_hndl = NULL;

/* Send CAN messagees */
static st_can_msg g_msg_nnn = { E_CAN_ID_NNN, E_CAN_KIND_STD, E_CAN_DLC_4, E_CAN_NNN_INIT };
static st_can_msg g_msg_yyy = { E_CAN_ID_YYY, E_CAN_KIND_STD, E_CAN_DLC_8, E_CAN_YYY_INIT };

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno ntm_create_task( void )
{
    BaseType_t result = pdFAIL;

    g_evt_hndl = xEventGroupCreate();

    if( NULL != g_evt_hndl )
    {
        g_delivery_timer_hndl = xTimerCreate("CAN_DELIVERY", DELIVERY_PERIOD,
            pdTRUE , NULL, delivery_cbk);
    }

    if( NULL != g_delivery_timer_hndl )
    {
        g_send_queue_hndl = xQueueCreate( SEND_QUEUE_ELEM_MAX, SEND_QUEUE_ELEM_SIZE );;
    }

    if( NULL != g_send_queue_hndl )
    {
        result = xTaskCreate( task, "CAN_MANAGER", 1024, NULL, E_TASK_PRIO_CAN, &g_tsk_hndl );
    }

    return ( pdPASS == result ) ? E_OK : E_NOK;
}

/* -------------------------------------------------------------------------- */
/* Private function                                                           */
/* -------------------------------------------------------------------------- */
static void task( void *nouse )
{
    EventBits_t events;

    reset_controller();

    (void)xTimerStart( g_delivery_timer_hndl, DELIVERY_PERIOD ); 

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
                proc_recv_can( E_CAN_RX_1 );

                /* Enable CAN IRQ factor of the RX1 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_RX1 );
            }

            if( (EventBits_t)E_NTM_EVT_RECV_RX2 == ( (EventBits_t)E_NTM_EVT_RECV_RX2 & events ) )
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
    typedef struct
    {
        en_can_irq_fact fact;
        en_ntm_event evt;
    } st_fact_to_evt;

    const st_fact_to_evt T_CONV[] =
    {
        { E_CAN_IRQ_FACT_RX1, E_NTM_EVT_RECV_RX1 },
        { E_CAN_IRQ_FACT_RX2, E_NTM_EVT_RECV_RX2 },
        { E_CAN_IRQ_FACT_TX1, E_NTM_EVT_SENT_TX1 },
        { E_CAN_IRQ_FACT_TX2, E_NTM_EVT_SENT_TX2 },
        { E_CAN_IRQ_FACT_TX3, E_NTM_EVT_SENT_TX3 }
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
    const uint8_t C_FACT = (E_CAN_IRQ_FACT_RX1 | E_CAN_IRQ_FACT_RX2 |
        E_CAN_IRQ_FACT_TX1 | E_CAN_IRQ_FACT_TX2 | E_CAN_IRQ_FACT_TX3 );

    /* Reset CAN controller */
    hwd_reset_can_controller();

    /* Set IRQ handler */
    hwd_set_can_irq_cbk( irq_handler );

    /* Enable CAN IRQ factor */
    hwd_enable_can_irq_fact( C_FACT );

    /* Enable CAN IRQ */
    hwd_enable_irq_handling( true );

    /* Start CAN communication */
    hwd_start_can_comm();
}

static void proc_recv_can( const en_can_rx can_rx )
{
    st_can_msg can_msg = { CAN_ID_INVAL, E_CAN_KIND_INVAL, E_CAN_DLC_MIN, { 0U } };

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

static void delivery_cbk( const TimerHandle_t hndl )
{
    const TickType_t C_QUEUE_WAIT = pdMS_TO_TICKS( SEND_QUEUE_WAIT_MS );

    typedef enum
    {
        E_PERIOD_NNN = ( 1000U / DELIVERY_PERIOD_MS ),
        E_PERIOD_YYY = (   10U / DELIVERY_PERIOD_MS )
    } en_period;

    typedef struct
    {
        const en_can_id id;
        const en_period period;
        uint16_t remaining;
        st_can_msg *p_msg;
    } st_count;

    st_count count_tbl[] =
    {
        { .id = E_CAN_ID_NNN, .period = E_PERIOD_NNN, .remaining = E_PERIOD_NNN, .p_msg = &g_msg_nnn },
        { .id = E_CAN_ID_YYY, .period = E_PERIOD_YYY, .remaining = E_PERIOD_YYY, .p_msg = &g_msg_yyy }
    };

    uint8_t idx;
    st_count *p_cnt;
    BaseType_t result;

    for( idx = 0U; idx < sizeof( count_tbl ); idx++ )
    {
        p_cnt = &count_tbl[ idx ];

        if( NO_PERIODIC != p_cnt->period )
        {
            if( NO_REMAINING != p_cnt->remaining )
            {
                p_cnt->remaining--;
            }
            else
            {
                result = xQueueSendToBack( g_send_queue_hndl, &p_cnt->id, C_QUEUE_WAIT );

                if( pdPASS == result )
                {
                    p_cnt->remaining = p_cnt->period;
                }
                else
                {
                    break;
                }
            }
        }
    }
}