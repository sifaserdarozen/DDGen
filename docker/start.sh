#!/bin/sh

set -ex

sh ./cert-generate.sh

exec "$@"
