/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* FreeRTOS */
#include <FreeRTOS.h>
#include <semphr.h>

#include <task_cfg.h>
#include <CanTxManager.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define NO_WAIT (0U)
#define NO_AVAILABLE (0U)

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */
typedef SemaphoreHandle_t (*getSemphrFunc)(void);

typedef struct
{
    en_can_tx canTx;
    getSemphrFunc getSemphr;
} stCanTxManage;

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void task(void *nouse);
static SemaphoreHandle_t getTx1Semphr(void);
static SemaphoreHandle_t getTx2Semphr(void);
static SemaphoreHandle_t getTx3Semphr(void);
static void releaseCanTxSemphr(SemaphoreHandle_t semphr);

/* -------------------------------------------------------------------------- */
/* Constant                                                                   */
/* -------------------------------------------------------------------------- */
static const stCanTxManage cTxSemphrTbl[E_CAN_TX_QTY] = {
    {E_CAN_TX_1, getTx1Semphr},
    {E_CAN_TX_2, getTx2Semphr},
    {E_CAN_TX_3, getTx3Semphr}};

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static TaskHandle_t gTask = NULL;
static volatile SemaphoreHandle_t gTx1Semphr = NULL;
static volatile SemaphoreHandle_t gTx2Semphr = NULL;
static volatile SemaphoreHandle_t gTx3Semphr = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
en_errno CreateCanTxManagerTask(void)
{
    gTx1Semphr = xSemaphoreCreateBinary();
    gTx2Semphr = xSemaphoreCreateBinary();
    gTx3Semphr = xSemaphoreCreateBinary();

    for (uint8_t i = 0U; i < E_CAN_TX_QTY; i++)
    {
        SemaphoreHandle_t semphr = cTxSemphrTbl[i].getSemphr();

        /* Return error if failed semaphore creation */
        if (NULL == semphr)
        {
            return E_NOK;
        }

        /* Release semaphore */
        releaseCanTxSemphr(semphr);
    }

    BaseType_t result = xTaskCreate(task, "CanTxManager", 1024, NULL, E_TASK_PRIO_CAN_TX, &gTask);

    return (pdPASS == result) ? E_OK : E_NOK;
}

en_can_tx HoldCanTx(void)
{
    for (uint8_t i = 0U; i < E_CAN_TX_QTY; i++)
    {
        SemaphoreHandle_t semphr = cTxSemphrTbl[i].getSemphr();

        /* Hold TX if TX is released */
        if ((NULL != semphr) && (NO_AVAILABLE != uxSemaphoreGetCount(semphr)) &&
            (pdPASS == xSemaphoreTake(semphr, NO_WAIT)))
        {
            return cTxSemphrTbl[i].canTx;
        }
    }

    /* If all TX are held */
    return E_CAN_TX_INVALID;
}

void ReleaseCanTx(en_can_tx canTx)
{
    for (uint8_t i = 0U; i < E_CAN_TX_QTY; i++)
    {
        /* Release specified TX */
        if (canTx == cTxSemphrTbl[i].canTx)
        {
            releaseCanTxSemphr(cTxSemphrTbl[i].getSemphr());
            break;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* Private function                                                           */
/* -------------------------------------------------------------------------- */
static void task(void *nouse)
{
}

static void releaseCanTxSemphr(SemaphoreHandle_t semphr)
{
    /* Release semaphore if it held */
    if ((NULL != semphr) && (NO_AVAILABLE == uxSemaphoreGetCount(semphr)))
    {
        (void)xSemaphoreGive(semphr);
    }
}

static SemaphoreHandle_t getTx1Semphr(void)
{
    return gTx1Semphr;
}

static SemaphoreHandle_t getTx2Semphr(void)
{
    return gTx2Semphr;
}

static SemaphoreHandle_t getTx3Semphr(void)
{
    return gTx3Semphr;
}
