#!/bin/sh

#
# $1 Source Nodes Name
# $2 Target Nodes Name
#

curl -vvv -XDELETE http://localhost:10000/manage/node/connect/ -d "{\"source_name\":\"$1\",\"target_name\":\"$2\"}"
