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
    E_CAM_EVT_NONE  = 0x000000U,
    E_CAM_EVT_RESET = 0x000001U,
    E_CAM_EVT_ALL   = E_CAM_EVT_RESET
} cam_event_t;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void task( void *nouse );
static void irq_handler( void );
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

        if( E_CAM_EVT_NONE != ( E_CAM_EVT_RESET & events ) )
        {
            /* Reset CAN controller */
            reset_controller();
        }
        else
        {
            // Others
        }
    }

}

static void irq_handler( void )
{
    static BaseType_t hptw = pdFALSE;

    /* Prevent continuous occurs IRQ */
    hwd_enable_can_irq( false );

    /* Notify to task 'CAN_MANAGER' */
    xEventGroupSetBitsFromISR( g_evt_hndl, E_CAM_EVT_RESET, &hptw );

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