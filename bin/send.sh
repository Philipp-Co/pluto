#!/bin/sh
#
# Send an Event to a specific Node.
#
# Args:
#   $1: Name of Node.
#   $2: Payload.
#
curl -vvv -XPOST http://127.0.0.1:10000/runtime/events/ -d '{"id":1,"event_id":2,"timestamp":"2026-01-01T00:00:00.0+00:00","payload":"$2", "target_node_name": "$1"}'
