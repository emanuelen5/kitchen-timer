#!/usr/bin/env bash

if [ $# -lt 2 ]; then
    echo "Usage:$0 <programmer> <environment>" >&2
    echo "  - programmer: any of the Avrdude programmers (see the the output of \`avrdude -c ?\`)" >&2
    echo "  - environment: any of the PlatformIO environments" >&2
    exit 1
fi

avrdude_programmer="$1"
environment="$2"

set -ex

pio run -e "$environment"
avrdude -p atmega328p -c "$avrdude_programmer" -U flash:w:".pio/build/$environment/firmware.hex":i
