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
#include <semphr.h>

#include <app_errno.h>
#include <hwd_api.h>
#include <task_cfg.h>
#include <can.h>
#include <private/ntm_can_msg.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define QUE_WAIT_TICK                   ( (TickType_t)0U )
#define SEMPHR_WAIT_TICK                ( (TickType_t)0U )

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
static void task( void* nouse );
static void irq_handler( const uint8_t fact );
static void reset_controller( void );
static void proc_recv_can( const en_can_rx can_rx );
static void delivery_cbk( const TimerHandle_t hndl );
void send_can_msg( void );

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static TaskHandle_t g_tsk_hndl = NULL;
static EventGroupHandle_t g_evt_hndl = NULL;
static TimerHandle_t g_delivery_timer_hndl = NULL;
static volatile QueueHandle_t g_send_que_hndl = NULL;
static volatile SemaphoreHandle_t g_tx1_semphr_hndl = NULL;
static volatile SemaphoreHandle_t g_tx2_semphr_hndl = NULL;
static volatile SemaphoreHandle_t g_tx3_semphr_hndl = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno ntm_create_task( void )
{
    const UBaseType_t C_QUE_ITEM_QTY = 10U;
    const UBaseType_t C_QUE_ITEM_SIZE = sizeof( st_can_msg* );
    const TickType_t C_DELIVERY_PERIOD_TICK = pdMS_TO_TICKS( 10U );

    BaseType_t result = pdFAIL;

    g_evt_hndl = xEventGroupCreate();

    result = ( NULL != g_evt_hndl ) ? pdPASS : pdFAIL;

    if( pdPASS == result )
    {
        g_delivery_timer_hndl = xTimerCreate(
            "CAN_DELIVERY", C_DELIVERY_PERIOD_TICK, pdTRUE , NULL, delivery_cbk);

        result = ( NULL != g_delivery_timer_hndl ) ? pdPASS : pdFAIL;
    }

    if( pdPASS == result )
    {
        g_send_que_hndl = xQueueCreate( C_QUE_ITEM_QTY, C_QUE_ITEM_SIZE );

        result = ( NULL != g_send_que_hndl ) ? pdPASS : pdFAIL;
    }

    if( pdPASS == result )
    {
        g_tx1_semphr_hndl = xSemaphoreCreateBinary();

        result = ( NULL != g_tx1_semphr_hndl ) ? pdPASS : pdFAIL;
    }

    if( pdPASS == result )
    {
        g_tx2_semphr_hndl = xSemaphoreCreateBinary();

        result = ( NULL != g_tx2_semphr_hndl ) ? pdPASS : pdFAIL;
    }

    if( pdPASS == result )
    {
        g_tx3_semphr_hndl = xSemaphoreCreateBinary();

        result = ( NULL != g_tx3_semphr_hndl ) ? pdPASS : pdFAIL;
    }

    if( pdPASS == result )
    {
        result = xTaskCreate( task, "CAN_MANAGER", 1024, NULL, E_TASK_PRIO_CAN, &g_tsk_hndl );
    }

    return ( pdPASS == result ) ? E_OK : E_NOK;
}

/* -------------------------------------------------------------------------- */
/* Private function                                                           */
/* -------------------------------------------------------------------------- */
static void task( void* nouse )
{
    const TickType_t C_DELIVERY_DELAY_TICK = 0U;

    EventBits_t events;
    BaseType_t result;

    ntm_init_can_msg_buff();

    reset_controller();

    (void)xSemaphoreGive( g_tx1_semphr_hndl );
    (void)xSemaphoreGive( g_tx2_semphr_hndl );
    (void)xSemaphoreGive( g_tx3_semphr_hndl );

    (void)xTimerStart( g_delivery_timer_hndl, C_DELIVERY_DELAY_TICK ); 

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

            if( (EventBits_t)E_NTM_EVT_SENT_TX1 == ( (EventBits_t)E_NTM_EVT_SENT_TX1 & events ) )
            {
                (void)xSemaphoreGive( g_tx1_semphr_hndl );

                /* Enable CAN IRQ factor of the TX1 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_TX1 );
            }

            if( (EventBits_t)E_NTM_EVT_SENT_TX2 == ( (EventBits_t)E_NTM_EVT_SENT_TX2 & events ) )
            {
                (void)xSemaphoreGive( g_tx2_semphr_hndl );

                /* Enable CAN IRQ factor of the TX2 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_TX2 );
            }

            if( (EventBits_t)E_NTM_EVT_SENT_TX3 == ( (EventBits_t)E_NTM_EVT_SENT_TX3 & events ) )
            {
                (void)xSemaphoreGive( g_tx3_semphr_hndl );

                /* Enable CAN IRQ factor of the TX3 */
                hwd_enable_can_irq_fact( (uint8_t)E_CAN_IRQ_FACT_TX3 );
            }

            send_can_msg();
        }
    }
}

void send_can_msg( void )
{
    typedef struct
    {
        en_can_tx idx;
        SemaphoreHandle_t semphr;
    } tx_cfg;

    const tx_cfg T_TX_CFG[] =
    {
        { E_CAN_TX_1, g_tx1_semphr_hndl },
        { E_CAN_TX_2, g_tx2_semphr_hndl },
        { E_CAN_TX_3, g_tx3_semphr_hndl }
    };

    st_can_msg* p_msg = &g_can_inval;

    if( pdPASS == xQueueReceive( g_send_que_hndl, &p_msg, QUE_WAIT_TICK ) )
    {
        for( uint8_t i = 0U; i < E_CAN_TX_QTY; i++ )
        {
            if ( pdTRUE == xSemaphoreTake( T_TX_CFG[ i ].semphr, SEMPHR_WAIT_TICK ) )
            {
                hwd_set_can_msg( T_TX_CFG[ i ].idx, p_msg );

                break;
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
    
    const uint8_t C_QTY = sizeof( T_CONV ) / sizeof( st_fact_to_evt );

    uint8_t idx;
    uint8_t irq;
    BaseType_t hptw = pdFALSE;

    for( idx = 0U; idx < C_QTY; idx++ )
    {
        if( T_CONV[ idx ].fact == ( T_CONV[ idx ].fact & fact ) )
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
    st_can_msg can_msg = { CAN_ID_INVAL, E_CAN_KIND_INVAL, E_CAN_DLC_0, { 0U } };

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
    const uint16_t C_NO_REMAINING = 0U;

    typedef struct
    {
        bool is_comp;
        en_can_period period;
        uint16_t remaining;
        st_can_msg* p_msg;
    } st_count;

    static st_count count_tbl[] =
    {
        { .is_comp = false, .period = E_CAN_246_PERIOD, .remaining = E_CAN_246_PERIOD, .p_msg = &g_can_246 },
        { .is_comp = false, .period = E_CAN_428_PERIOD, .remaining = E_CAN_428_PERIOD, .p_msg = &g_can_428 }
    };

    uint8_t idx;
    st_count* p_cnt;
    BaseType_t result;

    for( idx = 0U; idx < ( sizeof( count_tbl ) / sizeof( st_count ) ); idx++ )
    {
        p_cnt = &count_tbl[ idx ];

        if( E_NO_PERIOD != p_cnt->period )
        {
            /* Countdown the remaining */
            if( C_NO_REMAINING != p_cnt->remaining )
            {
                p_cnt->remaining--;
            }
            else
            {
                p_cnt->is_comp = true;
                p_cnt->remaining = p_cnt->period;
            }

            /* Queue messages whose sending period has expired */
            if( true == p_cnt->is_comp )
            {
                result = xQueueSendToBack( g_send_que_hndl, &p_cnt->p_msg, QUE_WAIT_TICK );

                if( pdPASS == result )
                {
                    p_cnt->is_comp = false;
                }
                else
                {
                    break;
                }
            }
        }
    }
}