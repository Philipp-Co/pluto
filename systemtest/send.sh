#!/bin/sh
for i in 1 2 3 4 5; do
    curl -vvv -XPOST http://127.0.0.1:10000/runtime/events/ -d '{"id":1,"event_id":2,"timestamp":"2026-01-01T00:00:00.0+00:00","payload":"YWhhCg=="}'
done
