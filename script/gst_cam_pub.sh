#!/bin/bash

PRIORITY="Data"
EXPRESS=""

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --priority) PRIORITY="$2"; shift ;;
        --no-express) EXPRESS="true" ;;
        *) echo "未知參數: $1"; exit 1 ;;
    esac
    shift
done

if [[ -n "$PRIORITY" ]]; then
    TEXTOVERLAY="textoverlay text=\"$PRIORITY\" valignment=top halignment=left font-desc=\"Sans, 36\" !"
else
    TEXTOVERLAY=""
fi

ZN_PRIORITY=""
if [[ -n "$PRIORITY" ]]; then
    ZN_PRIORITY="--priority $PRIORITY"
fi

ZN_EXPRESS=""
if [[ -n "$EXPRESS" ]]; then
    ZN_EXPRESS="--no-express"
fi

gst-launch-1.0 v4l2src device=/dev/video0 ! \
    image/jpeg,width=1280,height=720 ! \
    jpegdec ! videoconvert ! \
    timeoverlay valignment=4 halignment=1 ! tee name=t \
    t. ! queue ! xvimagesink sync=0 \
    t. ! queue ! nvvideoconvert ! nvv4l2h265enc idrinterval=15 ! filesink location=/dev/stdout | \
    RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 --pub stream/$PRIORITY $ZN_PRIORITY $ZN_EXPRESS