#ifndef __PLUTO_EDGE_TEST_H__
#define __PLUTO_EDGE_TEST_H__

#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_message_queue.h>

extern PLUTO_Logger_t PLUTO_TEST_logger;
extern PLUTO_MessageQueue_t PLUTO_TEST_queue;

void PLUTO_TEST_EdgeInitial(void);
void PLUTO_TEST_EdgeSendAndReceive(void);

#endif


