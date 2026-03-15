#!/bin/sh
#
# Send an Event to a specific Node.
#
# Args:
#   $1: Name of Node.
#   $2: Payload.
#
TARGET_NODE="$1"
PAYLOAD="$2"

echo "Start:\n"
date -u +"%Y-%m-%dT%H:%M:%S.%N%:z"

curl -vvv -X POST http://127.0.0.1:10000/runtime/events/ \
  -H "Content-Type: application/json" \
  -d "{
    \"id\": 1,
    \"event_id\": 2,
    \"timestamp\": \"2026-01-01T00:00:00.0+00:00\",
    \"payload\": \"${PAYLOAD}\",
    \"target_node_name\": \"${TARGET_NODE}\"
  }"
