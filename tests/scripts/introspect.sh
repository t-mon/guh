#!/bin/bash

if [ -z $2 ]; then
  echo "usage: $0 host token"
else
  TOKEN=$2
  (echo '{"id":1, "method": "JSONRPC.Introspect", "token": "'${TOKEN}'"}'; sleep 1) | nc $1 2222
fi
