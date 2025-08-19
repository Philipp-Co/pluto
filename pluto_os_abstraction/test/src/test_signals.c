#include "pluto/os_abstraction/pluto_malloc.h"
#include <pluto/os_abstraction/test/test_signals.h>
#include <pluto/os_abstraction/signals/pluto_signal.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <Unity/src/unity.h>
#include <signal.h>
#include <unistd.h>


void PLUTO_TEST_SignalsInitial(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("Signals");
    PLUTO_SignalHandler_t handler = PLUTO_CreateSignalHandler(logger); 
    TEST_ASSERT_NOT_NULL(handler);

    PLUTO_SignalEvent_t event;
    TEST_ASSERT_EQUAL(
        false,
        PLUTO_SignalPendingEvent(handler, &event)
    );

    PLUTO_DestroySignal(&handler); 
    TEST_ASSERT_NULL(handler);
    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
}

void PLUTO_TEST_SignalsAddListener(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("Signals");
    PLUTO_SignalHandler_t handler = PLUTO_CreateSignalHandler(logger); 
    TEST_ASSERT_NOT_NULL(handler);
    
    PLUTO_Signal_t listener = {
        .signal_numer=SIGUSR1
    };
    TEST_ASSERT_TRUE(
        PLUTO_SignalAddListener(handler, listener)
    );
    kill(getpid(), SIGUSR1);
    usleep(1000);
    PLUTO_SignalEvent_t event;
    TEST_ASSERT_TRUE(
        PLUTO_SignalPendingEvent(handler, &event)
    );

    PLUTO_DestroySignal(&handler); 
    TEST_ASSERT_NULL(handler);
    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
}

void PLUTO_TEST_SignalsRemoveListener(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("Signals");
    PLUTO_SignalHandler_t handler = PLUTO_CreateSignalHandler(logger); 
    TEST_ASSERT_NOT_NULL(handler);
    
    PLUTO_Signal_t listener = {
        .signal_numer=SIGUSR1
    };
    TEST_ASSERT_TRUE(
        PLUTO_SignalAddListener(handler, listener)
    );
    PLUTO_SignalRemoveListener(handler, listener);

    kill(getpid(), SIGUSR1);
    usleep(1000);
    PLUTO_SignalEvent_t event;
    TEST_ASSERT_FALSE(
        PLUTO_SignalPendingEvent(handler, &event)
    );

    PLUTO_DestroySignal(&handler); 
    TEST_ASSERT_NULL(handler);
    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
}
