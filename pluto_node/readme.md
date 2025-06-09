# pluto_node

pluto_node ist eine Anwendung. pluto_node definiert eine Schnittstelle ueber die eine Fachlogik in die Anwendung injiziert werden kann.

## Protokol

Pluto definiert Events als Textnachrichten in folgendem Schema.

    {
        "id": int,
        "event": int,
        "time": datetime, # %Y-%m-%dT%H:%M:%S.%f%z
        "payload": str
    }

Die "id" ist eine Benutzerdefinierte Zahl und sollte sogewaehlt, dass sie aus Benutzersicht eindeutig ist.
"queue" beinhaltet die Nummer der, fuer ein erzeugtes Folgeevent, zu verwendenden Ausgabequeue. 
Im "payload" werden die zum Event gehoerenden Daten hinterlegt.

## Infrastruktur

pluto_node verwendet genau eine IPC faehige Queue als Eingabequelle fuer seine Events und eine konfigurierbare Anzahl an IPC faehgigen Queues als Ausgabe fuer die von der ausgefuehrten Fachlogik erzeugten Events.

## Verhalten

pluto_node baut die notwendige IPC Infrastruktur auf die sie benoetigt und verwendet bestehende Komponenten um sich sinnvoll in das Gesamtsystem zu integrieren.
pluto_node initialisiert die Fachlogik.
Fuer jedes eingehende Event fuehrt pluto_node die hinterlegte Fachlogik aus und leitet das neu produzierte Event entsprechend weiter.
Zum Programmende fuehrt pluto_node eine deinitialisierung der Fachlogik durch.

## Features

### Thread Pool

Pluto Node stellt einen Thread Pool bereit mit dem die Anwendung asynchrone Aufgabe erledigen kann.

    Node -> Handler -> Thread Pool -> Node

## Varianten

Varianten werden zur Compile Zeit definiert und entsprechend ueber Praeprozessor Makros definiert.
Aktuell stehen drei Varianten zur Verfuegung.

Die Makros werden in der Datei 

    pluto/inc/pluto/pluto_compile_time_switches.h

definiert und koennen durch das Buildsystem ueberschrieben werden.

    PLUTO_CTS_RUNTIME_MODE

Definiert welche Variante gebaut wird.
PLUTO_CTS_RUNTIME_MODE versteht diese Werte:
    
    PLUTO_CTS_RTM_PASSTHROUGH 0
    PLUTO_CTS_RTM_SHARED_LIB 1
    PLUTO_CTS_RTM_PYTHON 2

Die Funktionsweise wird im folgenden beschrieben.

### Passtrhrough

Diese Variante fuehrt keine Fachlogik aus. Sie leitet eingehende Events 1:1 weiter.

### Python

Diese Variante integriert einen Pythoninterpreter und ermoeglicht so, dass Fachlogik in Python implementiert werden kann.

### Shared Library

Diese Variante implementiert eine Moeglichkeit die Fachlogik ueber eine Shared Library einzubinden.

## Laufzeit

### Konfiguration

Die Konfigurationsdatei muss diesem Schema entsprechen.

    {
        "work_dir": str,                // Ein Pfad zu einem Verzeichnis.
        "name_of_input_queue": str,     // Der Name der Eingabequeue.
        "names_of_output_queues": [str],// Eine Liste mit Namen von Ausgabequeues.
        Optional["python_path": str]    // Eine durch ";" getrennte Liste aus Pfaden.
    }

### Ausfuehren
   
Konfiguration unter /var/pluto_example/config-pluto-0.txt erstellt.

    {
        "work_dir": "/var/pluto_example/pluto-0/",
        "name": "pluto-0" 
    }

Mit dem Kommando starten.
 
    ./pluto_node_pt -n pluto-0 -c /var/pluto_example/config-pluto-0.txt

Eingabe schreiben.

    ./pluto_rw -n /var/pluto_example/pluto-0/pluto-0_iq -w -d '{"id":0, "event":0,  "time": """payload":"test"}'

Eingabe lesen.

    ./pluto_rw -n /var/pluto_example/pluto-0/pluto-0_oq_0 -r    // Gibt '{"id":0, "payload":test}'

## Architektur

    +-------------------------------+   
    |   application_interface_layer |
    +-------------------------------+ 
    |   application_layer           |   python  shared_library 
    +-------------------------------+
    |   os_abstraction_layer        |
    +-------------------------------+


