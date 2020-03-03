#!/bin/sh

debugsym=true
dynamodb=false
s3=false


for arg in "$@"; do
    case "$arg" in
    --use-dynamodb)
        dynamodb=true;;
     --use-s3)
        s3=true;;
    --enable-debug)
        debugsym=true;;
    --disable-debug)
        debugsym=false;;

    --help)
        echo 'usage: ./configure [options]'
        echo 'options:'
        echo '  --use-dynamodb: use dynamodb to archive calls'
        echo '  --use-s3: use s3 to archive generated pcaps'
        echo '  --enable-debug: include debug symbols'
        echo '  --disable-debug: do not include debug symbols'
        echo 'all invalid options are silently ignored'
        exit 0
        ;;
    esac
done


echo 'generating makefile ...'

touch makefile

echo "# Configuration parameters chosen ..." > makefile

if "$dynamodb" || "$s3"; then
    echo ".calling cmake..."
    cd "3rdParty/aws-sdk"
    mkdir _build
    cd _build
    cmake -D BUILD_ONLY="s3;dynamodb" -D BUILD_SHARED_LIBS="OFF" -D ENABLE_TESTING="OFF" ../aws-sdk-cpp-1.7.267
    cd ../../..
fi

if "$dynamodb"; then
    echo "DYNAMODB = $dynamodb" >> makefile
fi
if "$s3"; then
    echo "S3 = $s3" >> makefile
fi
if "$debugsym"; then
    echo 'dbg = -g' >> makefile
fi

echo "" >> makefile

cat makefile.in >> makefile
echo 'configuration complete, type make to build.'
