#!/bin/bash

if [ -z $1 ]; then
  echo "usage $0 host"
else
  (echo '{"id":1, "method":"Authentication.Authenticate", "params": { "clientDescription": "Test script", "userName": "guh", "password": "guhh" } }'; sleep 1) | nc $1 2222
fi
