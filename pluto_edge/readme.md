# pluto_edge

pluto_edge ist eine Bibliothek die den Zugriff auf die von Pluto erstellte Infrastruktur bietet.

## Bauen

Dieses Unterprojekt wird auf hoechter Ebene von Top-Level CMake eingebnunden und gebaut.

    cd build/ && make pluto_edge

pluto_edge liegt nach dem Bauen in pluto_edge/ als statische Bibliothek "pluto_edge/pluto_edge_static" und als dynamisch Bibliothek "pluto_edge/pluto_edge_shared" vor.

## Tests

   cd build/ && ./pluto_edge/test/pluto_edge_test 

## pluto_edge Verwenden

### CMake

    target_include_directories(
        your_app
        path/to/pluto/pluto_edge/inc/
    )

    target_link_directories(
        your_app
        PUBLIC
        path/to/build_dir/pluto_edge/
    )
    
    target_link_libraries(
        your_app
        PUBLIC
        pluto_edge_static   # Or pluto_edge_shared if you want to link dynamically. 
    )

### Source Code 

    #include <pluto/pluto_edge/pluto_edge.h>

    ...
    PLUTO_EDGE_Queue_t queue = PLUTO_EDGE_CreateQueue("path/to/some/input_queue");
    PLUTO_EDGE_Event_t event = PLUTO_EDGE_CreateEvent();
    snprintf(
        PLUTO_EDGE_EventPayload(event),
        PLUTO_EDGE_EventPayloadBufferSize,
        "{\"attr\":\"yiha\",\"a_num\":42}"
    );
    PLUTO_EDGE_QueueWrite(queue, event);
    PLUTO_EDGE_DestroyEvent(event);
    ...
