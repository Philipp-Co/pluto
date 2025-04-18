# pluto_event

pluto_event ist eine Bibliothek die sich um dsa Eventhandling kuemmert.
Dazu gehoeren

    - Schema
        - Definition des Eventschemas
        - Bereitstellen eines C Structs auf welches das Schema abgebildet werden kann
    - Einen Parser
        - Load
        - Dump 

## Event Schema

    {
        "id": int,
        "event": int,
        "time": str,    # %Y-%m-%dT%H:%M:%S.%f%z -> 2025-01-01T00:00:00.0+000
        "payload": str,
    }

## Parser

Der Parser wird mit Hilfe von jsmn umgesetzt.
