#!/bin/sh

#
# $1 - Nodename
# $2 - Path to .tar.gz
# $3 - Toplevel Package Name
#

curl -vvv -XPUT http://localhost:10000/manage/node/add/$1/package/ -d "{\"content\":\"$(cat $2 | openssl base64 -A)\"}"
curl -vvv -XPUT http://localhost:10000/manage/node/add/$1/ -d "{\"top_level_package_name\":\"$3\"}"
