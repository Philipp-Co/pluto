#!/bin/sh

handle_sigint() {
    echo 'SIGINT to $1 and $2'
    kill -INT $1
    kill -INT $2
    wait $1
    wait $2
}

#
# Run pluto_core - the main Application.
#
(
    pluto_core -c /pluto/core/config/core.cfg
) &
_pid_pluto_core=$!

#
# Run the HTTP Interface.
#
(
    cd /pluto_http_interface/ && \
        . .venv/bin/activate && \
        cd plyto_http_interface/ && \
        gunicorn plyto_http_interface.wsgi:application \
            --bind 0.0.0.0:8000 \
            --workers 2 \
            --worker-class gthread \
            --threads 4
) &
_pid_http_interface=$!

#
# Define Traps for SIGINT and SIGTERM.
#
trap 'handle_sigint $_pid_http_interface $_pid_pluto_core' INT 
trap 'handle_sigint $_pid_http_interface $_pid_pluto_core' TERM 

#
# Wait...
#
wait $_pid_pluto_core
wait $_pid_http_interface
