# os_abstraction

Dies ist Plutos Betriebssystem Abstraktionsschicht.
Hier werden alle Funktionen implementiert die eigentlich vom Betriebssystem bereit gestellt werden. 
Das sind Dinge wie

    - Semaphoren
    - Message Queues

Zusaetzlich wird hier auch eine Fassade fuer einige Teile der Std Lib umgesetzt.
Das gilt zum Beispiel fuer

    - time.h / sys/time.h
    - malloc und free 

## Compile Definitions

    PLUTO_TEST

Definierte dieses Makro um eine Variante zu complieren die um einige Testfunktionen erweitert ist.

## Einbinden

    target_link_directories(
        some_target
        PUBLIC
        ...
        ${CMAKE_CURRENT_LIST_DIR}/../pfad/zu/os_abstraction/build/dir/
    )
    
    target_link_libraries(
        some_target
        PUBLIC
        ...
        os_abstraction
    )

    target_include_directories(
        some_target
        PUBLIC
        ...
        ${CMAKE_CURRENT_LIST_DIR}/../pfad/zu/os_abstraction/inc/
    )

