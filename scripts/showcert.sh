#!/bin/bash
# This is a useful script that you can run to see details about a PEM formatted certificate

set -e

openssl x509 -in $1 -noout -text

fpsha256=$(openssl x509 -noout -fingerprint -sha256 -inform pem -in $1)
fpsha1=$(openssl x509 -noout -fingerprint -inform pem -in $1)

echo ${fpsha256//:/}
echo ${fpsha1//:/}
