#!/bin/sh
curl --silent -N -XGET http://127.0.0.1:10000/runtime/events/ | grep -v '^:' | grep -v '^$'
