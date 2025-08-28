#include <pluto/os_abstraction/system_events/pluto_system_events.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <Unity/src/unity.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void PLUTO_TEST_SystemEventsInitial(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("System Events");
    PLUTO_SystemEventHandler_t handler = PLUTO_CreateSystemEventHandler(logger);
    PLUTO_SystemEvent_t event = PLUTO_CreateSystemEvent();
    TEST_ASSERT_EQUAL(PLUTO_SE_NO_EVENT, PLUTO_SystemEventsPoll(handler, event));

    PLUTO_DestroySystemEvent(&event);
    PLUTO_DestroySystemEventHandler(&handler);
    TEST_ASSERT_NULL(event);
    TEST_ASSERT_NULL(handler);

    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
} 

void PLUTO_TEST_SystemEventsFileEvent(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("System Events");
    PLUTO_SystemEventHandler_t handler = PLUTO_CreateSystemEventHandler(logger);
    PLUTO_SystemEvent_t event = PLUTO_CreateSystemEvent();
    
    char path[1024];
    snprintf(path, sizeof(path), "%ssystem_event_dummy.txt", PLUTO_TEST_RESSOURCE_DIR);
    int filedescriptor = open(path, O_RDWR | O_CREAT, 0777);
    TEST_ASSERT_TRUE(filedescriptor >= 0);
    TEST_ASSERT_EQUAL(
        0,
        PLUTO_SystemEventHandlerRegisterFileObserver(handler, path)
    );
    
    char buffer[1024]; 
    snprintf(buffer, sizeof(buffer), "test");
    write(filedescriptor, buffer, strlen(buffer));
    //
    // since events are async, we have to wait a little bit...
    //
    time_t start, current;
    time(&start);
    int result = PLUTO_SE_NO_EVENT;
    while(PLUTO_SE_OK != result)
    {
        time(&current); 
        if(difftime(current, start) > 1.0f)
        {
            break;
        }
        result = PLUTO_SystemEventsPoll(handler, event);
    }
    TEST_ASSERT_EQUAL(PLUTO_SE_OK, result);
    close(filedescriptor);
    remove(path);
    PLUTO_DestroySystemEvent(&event);
    PLUTO_DestroySystemEventHandler(&handler);
    TEST_ASSERT_NULL(event);
    TEST_ASSERT_NULL(handler);

    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
} 

void PLUTO_TEST_SystemEventsDeregisterFileObserver(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("System Events");
    PLUTO_SystemEventHandler_t handler = PLUTO_CreateSystemEventHandler(logger);
    PLUTO_SystemEvent_t event = PLUTO_CreateSystemEvent();
    
    char path[1024];
    snprintf(path, sizeof(path), "%ssystem_event_dummy.txt", PLUTO_TEST_RESSOURCE_DIR);
    int filedescriptor = open(path, O_RDWR | O_CREAT, 0777);
    TEST_ASSERT_TRUE(filedescriptor >= 0);
    TEST_ASSERT_EQUAL(
        0,
        PLUTO_SystemEventHandlerRegisterFileObserver(handler, path)
    );
    TEST_ASSERT_EQUAL(0, PLUTO_SystemEventsHandlerDeregisterFileObserver(handler, filedescriptor));
    
    char buffer[1024]; 
    snprintf(buffer, sizeof(buffer), "test");
    write(filedescriptor, buffer, strlen(buffer));
    //
    // since events are async, we have to wait a little bit...
    //
    sleep(1);

    TEST_ASSERT_EQUAL(PLUTO_SE_NO_EVENT, PLUTO_SystemEventsPoll(handler, event));

    close(filedescriptor);
    remove(path);
    PLUTO_DestroySystemEvent(&event);
    PLUTO_DestroySystemEventHandler(&handler);
    TEST_ASSERT_NULL(event);
    TEST_ASSERT_NULL(handler);

    // Destroy Logger and Check Malloc...
    PLUTO_DestroyLogger(&logger);
} 
