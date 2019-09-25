#! /bin/sh

if [ -z $1  ]
then
    file_name="ddgen-cert.pem"
else
    file_name=$1
fi

if [ -f $file_name ]
then
    echo "$file_name exists"
else
    echo "$file_name not exists, creating one"
    password=password
    openssl genrsa -des3 -passout pass:$password -out ddgen.key.orig 2048 -noout

    country="XX"
    state="XX"
    locality="XX"
    organization="XX"
    organizationalunit="XX"
    commonname="XX"
    email="XX"

    openssl req -new -key ddgen.key.orig -out ddgen.csr -passin pass:$password \
    -subj "/C=$country/ST=$state/L=$locality/O=$organization/OU=$organizationalunit/CN=$commonname/emailAddress=$email"

    openssl rsa -in ddgen.key.orig -out ddgen.key -passin pass:$password
    openssl x509 -req -days 3650 -in ddgen.csr -signkey ddgen.key -out ddgen.crt
    cp ddgen.crt $file_name
    cat ddgen.key >> $file_name

    rm ddgen.*
fi
