#!/bin/bash


(
    /bin/pluto_node_pt -n pluto-0 -c /pluto-0-config.txt
) &
_pid_0=$!
(
    /bin/pluto_node_pt -n pluto-1 -c /pluto-1-config.txt
) &
_pid_1=$!

trap "kill -SIGINT $_pid_0" SIGINT 
trap "kill -SIGINT $_pid_1" SIGINT 

wait $_pid_0
wait $_pid_1
