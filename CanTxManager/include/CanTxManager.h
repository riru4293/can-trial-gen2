#ifndef CAN_TX_MANAGER_H
#define CAN_TX_MANAGER_H

/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <app_errno.h>
#include <hwd_api.h>

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
en_errno CreateCanTxManagerTask(void);
en_can_tx HoldCanTx(void);
void ReleaseCanTx(en_can_tx canTx);

#endif
