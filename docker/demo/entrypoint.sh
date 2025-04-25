#!/bin/bash

echo "Starte Anwendung..."
echo "Starte Pluto Node 0."
(
    /bin/pluto_node_py -n pluto-0 -c /pluto-0-config.txt -e pluto_event_handler
) &
_pid_0=$!
echo "Starte Pluto Node 1."
(
    /bin/pluto_node_pt -n pluto-1 -c /pluto-1-config.txt
) &
_pid_1=$!
echo "Starte Pluto Node 2."
(
    /bin/pluto_node_sh -n pluto-2 -c /pluto-2-config.txt -e /pluto_shared_library_demo/build/libpluto_demo.so
) &
_pid_2=$!

#
# Catch ctrl-c.
#
trap "kill -SIGINT $_pid_0" SIGINT 
trap "kill -SIGINT $_pid_1" SIGINT 
trap "kill -SIGINT $_pid_2" SIGINT 

echo "Wait for $_pid_0, $_pid_1 and $_pid2"
wait $_pid_0
wait $_pid_1
wait $_pid_2

echo "Bye Bye."
