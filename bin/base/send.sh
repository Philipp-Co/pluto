#!/bin/sh
#
# Send an Event to a specific Node.
#
# Args:
#   $1: Name of Node.
#   $2: Payload.
#   $3: Id.
#   $4: Event-Id.
#
TARGET_NODE="$1"
PAYLOAD="$2"
ID="$3"
EVENT_ID="$4"

echo "Start:\n"
date -u +"%Y-%m-%dT%H:%M:%S.%N%:z"

curl -vvv -X POST http://127.0.0.1:10000/runtime/events/ \
  -H "Content-Type: application/json" \
  -d "{
    \"id\": ${ID},
    \"event_id\": ${EVENT_ID},
    \"timestamp\": \"2026-01-01T00:00:00.0+00:00\",
    \"payload\": \"${PAYLOAD}\",
    \"target_node_name\": \"${TARGET_NODE}\"
  }"
