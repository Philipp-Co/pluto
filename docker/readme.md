# Docker

## build-args

### PYTHON_PATH

PYTHON_PATH: Eine durch ";" getrennte Liste aus Pfaden die dem Python Path des eingebetteten Interpreters hinzugefuegt werden.

    PYTHON_PATH=/user/pluto/home/project/;/bin/

## Das Image bauen

Das hier angebotene Dockerimage ist als Basisimage gedacht. Das Image wird folgendermassen gebaut

    docker build -t pluto -f pluto/dockerfile ../

In diesem Image sind alle benoetigten Werkzeuge zum Erstellen von Plutos Komponenten.
Die Ausfuehrbahren Dateien werden unter /bin/ abgelegt

    - /bin/pluto_node_pt
    - /bin/pluto_node_py
    - /bin/pluto_node_sh

Die Header sind unter /usr/include/pluto/ abgelegt (werden zum Erstellen einer shared Library benoetigt).

## Demo   

In der Demo sind drei pluto_node Instanzen enthalten. Die da sind pluto_node_py, pluto_node_pt und pluto_node_sh.
Die Instanzen sind auch in dieser Reihenfolge miteinander verbunden.

       +---------+    +---------+    +---------+
    -> | pluto-0 | -> | pluto-1 | -> | pluto-2 | ->
       +---------+    +---------+    +---------+

Unter demo/ befindet sich eine einfache Demoanwendung die zeigt, wie das Basisimage verwendet werden kann um eine Anwendung zu integrieren.
Die Anwendung kann mit docker gebaut werden

    docker build -t pluto -f pluto/dockerfile ../ && docker build -t pluto_demo -f demo/dockerfile ../ && docker run -t pluto_demo

Nach dem Bauen muss ein Container mit dem Image pluto_demo gestartet werden.

    Terminal 1
    docker run pluto_demo

    Terminal 2
    docker ps
    # CONTAINER ID   IMAGE        COMMAND            CREATED         STATUS         PORTS     NAMES
    # d1977fd50ac1   pluto_demo   "/entrypoint.sh"   3 seconds ago   Up 2 seconds             optimistic_mclaren
    docker exec -it d1977fd50ac1 /bin/bash
    pluto_rw -n /pluto/ipc/pluto-0_iq -w -d '{"id":1,"event":1,"time":"2025-01-01T00:00:00.0+0000","payload":"test"}'
    pluto_rw -n /pluto/ipc/pluto-2_oq_0 -r
    # {"id":0,"payload":"{"shared_lib":{"id":0,"payload":"{"passthrough":"{"id":0,"payload":"{"python":"test"}"}"}"}}"}

