#!/bin/sh

if [[ -n "$1" && "$1" = "rm" ]]; then
  echo "Removing openssl files.";
  rm -rf `ls | sed "s/$(basename $0)//" | sed "s/openssl.cnf//"`;
  exit 0;
fi

echo "Creating openssl files."

mkdir -p certs
mkdir -p private
mkdir -p newcerts

echo 1000 > serial
touch index.txt

# Create CA.
openssl req -new -x509 -days 3650 -extensions v3_ca -keyout private/cakey.pem -out cacert.pem -config openssl.cnf

# Create the SSL request.
openssl req -new -nodes -out server-req.pem -keyout private/server-key.pem -config openssl.cnf

# Create the SSL certificate.
openssl ca -config openssl.cnf -out server-cert.pem -infiles server-req.pem

# Create the diffie-hellman
openssl dhparam -out dh1024.pem 1024
