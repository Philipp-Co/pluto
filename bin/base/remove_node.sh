#!/bin/sh

#
# $1 - Nodename
#

curl -vvv -XDELETE http://localhost:10000/manage/node/add/$1/ 
