/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
/* FreeRTOS */
#include <FreeRTOS.h>
#include <semphr.h>

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static volatile SemaphoreHandle_t gTx1Semphr = NULL;
static volatile SemaphoreHandle_t gTx2Semphr = NULL;
static volatile SemaphoreHandle_t gTx3Semphr = NULL;
