/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* System */
#include <stdio.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <queue.h>

/* APPL */
// #include "appl_common.h"
#include <my_types.h>
// #include <hardware_driver.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* Network manager events */
#define EVT_NMGR_NONE               ( (UINT32)0x000000U )
#define EVT_NMGR_ON_COMPLETED       ( (UINT32)0x000001U )
#define EVT_NMGR_OFF_COMPLETED      ( (UINT32)0x000002U )
#define EVT_NMGR_CAN_TX_ON_READY    ( (UINT32)0x000010U )
#define EVT_NMGR_CAN_TX_OFF_READY   ( (UINT32)0x000020U )
#define EVT_NMGR_CAN_RX_ON_READY    ( (UINT32)0x000040U )
#define EVT_NMGR_CAN_RX_OFF_READY   ( (UINT32)0x000080U )
#define EVT_NMGR_CAN_IRQ_ON_READY   ( (UINT32)0x000100U )
#define EVT_NMGR_CAN_IRQ_OFF_READY  ( (UINT32)0x000200U )
#define EVT_NMGR_ON_READY   ( (UINT32)( \
/* -- */(UINT32)EVT_NMGR_CAN_TX_ON_READY | (UINT32)EVT_NMGR_CAN_RX_ON_READY | (UINT32)EVT_NMGR_CAN_IRQ_ON_READY) )
#define EVT_NMGR_OFF_READY   ((UINT32)( \
/* -- */(UINT32)EVT_NMGR_CAN_TX_OFF_READY | (UINT32)EVT_NMGR_CAN_RX_OFF_READY | (UINT32)EVT_NMGR_CAN_IRQ_OFF_READY) )

/* CAN TX events */
#define EVT_CAN_TX_IND_NW_PRE_ON    ( (UINT32)0x000001U )
#define EVT_CAN_TX_IND_NW_PRE_OFF   ( (UINT32)0x000002U )

/* CAN RX events */
#define EVT_CAN_RX_IND_NW_PRE_ON    ( (UINT32)0x000001U )
#define EVT_CAN_RX_IND_NW_PRE_OFF   ( (UINT32)0x000002U )

/* CAN IRQ events */
#define EVT_CAN_IRQ_IND_NW_PRE_ON   ( (UINT32)0x000001U )
#define EVT_CAN_IRQ_IND_NW_PRE_OFF  ( (UINT32)0x000002U )

/* Network ON/OFF */
#define NETWORK_OFF ( (UINT8)0x00U )
#define NETWORK_ON  ( (UINT8)0x01U )

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static VOID nmgr_task( VOID* unused );
static VOID task2( VOID* unused );
ERR_CD_T req_network_on( VOID );
ERR_CD_T req_network_off( VOID );
static VOID turn_on_network( VOID );
static VOID turn_off_network( VOID );

/* -------------------------------------------------------------------------- */
/* Table                                                                      */
/* -------------------------------------------------------------------------- */
// const st_nw_switching_evt tbl_nw_switching_evt[] = {
//     { EVT_NMGR_OFF_COMPLETED, EVT_NMGR_CAN_TX_OFF_READY },
//     { EVT_NMGR_ON_COMPLETED , EVT_NMGR_CAN_TX_ON_READY  }
// };

/* -------------------------------------------------------------------------- */
/* Constant                                                                   */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
volatile static SemaphoreHandle_t g_semaphore;
static TaskHandle_t g_nmgr_tsk_hndl = NULL;
static TaskHandle_t g_can_tx_tsk_hndl = NULL;
static EventGroupHandle_t g_nmgr_evt_hndl;
static EventGroupHandle_t g_can_tx_evt_hndl;
static TimerHandle_t g_timer_handle;
static VOID timer_callback( TimerHandle_t timer_handler );
static BaseType_t xTimer2Started;
static volatile QueueHandle_t g_nmgr_req_queue = NULL;


/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
int main()
{
    /* Initialize hardware */
    // hwdrv_init_hardware();

    g_nmgr_evt_hndl = xEventGroupCreate();
    g_can_tx_evt_hndl = xEventGroupCreate();

    /* Create log queue */
    g_nmgr_req_queue = xQueueCreate( 1, 1 );

    xTaskCreate( nmgr_task, "NET_MGR", 1024, NULL, 1, &g_nmgr_tsk_hndl );
    xTaskCreate( task2, "CAN_TX", 1024, NULL, 2, &g_can_tx_tsk_hndl );

    vTaskStartScheduler();

    while ( TRUE ) {
        // NOP
    }
}

/*
 * CAN通信の開始を要求する
 *
 * args:    nothing
 * return:  E_OK    要求を受け付けた
 *          E_NOK    要求を拒否した
 * note:    CAN通信の開始もしくは終了を要求中は、要求を拒否する。
 */
ERR_CD_T req_network_on( VOID )
{
    static const UINT8 nw_on = NETWORK_ON;
    BaseType_t result;

    result = xQueueSendToBack( g_nmgr_req_queue, &nw_on, WAIT_NONE );

    return (pdPASS == result) ? E_OK : E_NOK;
}

/*
 * CAN通信の終了を要求する
 *
 * args:    nothing
 * return:  E_OK    要求を受け付けた
 *          E_NOK    要求を拒否した
 * note:    CAN通信の開始もしくは終了を要求中は、要求を拒否する。
 */
ERR_CD_T req_network_off( VOID )
{
    static const UINT8 nw_off = NETWORK_OFF;
    BaseType_t result;

    result = xQueueSendToBack( g_nmgr_req_queue, &nw_off, WAIT_NONE );

    return (pdPASS == result) ? E_OK : E_NOK;
}

/* -------------------------------------------------------------------------- */
/* Private function                                                           */
/* -------------------------------------------------------------------------- */
static VOID nmgr_task( VOID* unused )
{
    BaseType_t res;
    UINT8 req;

    /* Requests a network ON */
    (VOID)req_network_on();
    
    while( TRUE )
    {
        /* Wait a request that network ON or OFF */
        (VOID)xQueuePeek( g_nmgr_req_queue, &req, WAIT_FOREVER );

        switch( req )
        {
        case NETWORK_ON:
            turn_on_network();
            break;

        case NETWORK_OFF:
        default:
            turn_off_network();
            break;
            
        }

        /* Clear the queue that accepts network switching */
        do
        {
            res = xQueueReceive( g_nmgr_req_queue, &req, WAIT_NONE );
        } while ( pdPASS == res );

// == Test-code: Toggle the network ON/OFF ==
        switch( req )
        {
        case NETWORK_ON:
            (VOID)req_network_off();
            break;
        case NETWORK_OFF:
        default:
            (VOID)req_network_on();
            break;
        }
    }
// ==========================================
}

static VOID turn_on_network( VOID )
{
    /* Indicate pre network ON */
    (VOID)xEventGroupSetBits( g_can_tx_evt_hndl, EVT_CAN_TX_IND_NW_PRE_ON );
    // (VOID)xEventGroupSetBits( g_can_rx_evt_hndl, EVT_CAN_RX_IND_NW_PRE_ON );
    // (VOID)xEventGroupSetBits( g_can_irq_evt_hndl, EVT_CAN_IRQ_IND_NW_PRE_ON );

    /* Wait until all task is ready to network ON */
    (VOID)xEventGroupSync( g_nmgr_evt_hndl, EVT_NMGR_NONE, EVT_NMGR_ON_READY, WAIT_FOREVER );

    // ネットワークON
    vTaskDelay(1000U);

    printf("ON --> Done\n");

    /* Notificate network ON completion */
    (VOID)xEventGroupSetBits( g_nmgr_evt_hndl, EVT_NMGR_ON_COMPLETED );
}
static VOID turn_off_network( VOID )
{
    /* Indicate pre network OFF */
    (VOID)xEventGroupSetBits( g_can_tx_evt_hndl, EVT_CAN_TX_IND_NW_PRE_OFF );
    // (VOID)xEventGroupSetBits( g_can_rx_evt_hndl, EVT_CAN_RX_IND_NW_PRE_OFF );
    // (VOID)xEventGroupSetBits( g_can_irq_evt_hndl, EVT_CAN_IRQ_IND_NW_PRE_OFF );

    /* Wait until all task is ready to network OFF */
    (VOID)xEventGroupSync( g_nmgr_evt_hndl, EVT_NMGR_NONE, EVT_NMGR_OFF_READY, WAIT_FOREVER );

    // ネットワークOFF
    vTaskDelay(3000U);

    printf("OFF --> Done\n");

    /* Notificate network OFF completion */
    (VOID)xEventGroupSetBits( g_nmgr_evt_hndl, EVT_NMGR_OFF_COMPLETED );
}


static VOID task2( VOID* unused )
{
    EventGroupHandle_t my_hndl = g_can_tx_evt_hndl;
    EventGroupHandle_t mngr_hndl = g_nmgr_evt_hndl;
    EventBits_t events;
    
    while( TRUE )
    {
        events = xEventGroupWaitBits( my_hndl, 0x01U, pdTRUE, pdFALSE, WAIT_NONE );

        if(0x00 != (events & 0x01)) // 初期化要求
        {
            // 準備の代わり
            vTaskDelay(500U);

            // 準備完了を応答
            (VOID)xEventGroupSetBits( mngr_hndl, 0x08U );

            // 終了を待つ
            (VOID)xEventGroupSync(mngr_hndl, 0x00U, 0x04U, portMAX_DELAY);

            // 他の作業はしない
            continue;
        }
        else
        {
            vTaskDelay(500U);
            printf("CAN TX work\n");
        }

        printf("CAN TX work\n");
        vTaskDelay(500U);
    }
}
