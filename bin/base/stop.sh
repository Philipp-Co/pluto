#!/bin/sh
#
# Stop Pluto.
#
curl -vvv -XDELETE http://127.0.0.1:10000/manage/
