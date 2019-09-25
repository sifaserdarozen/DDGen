#!/bin/sh

set -ex

if [ `echo $@ | grep -c "\-\-secure" ` -gt 0 ]
then
    echo "Secure mode, doing a certificate check";
    sh ./cert-generate.sh
else
    echo "Non secure mode, wont do a certiciate check";
fi

exec "$@"
