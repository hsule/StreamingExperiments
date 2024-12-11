#!/bin/bash


PRIORITY="Data"
EXPRESS=""

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --priority) PRIORITY="$2"; shift ;;
        --express) EXPRESS="true" ;;
        *) echo "未知參數: $1"; exit 1 ;;
    esac
    shift
done

if [[ -n "$PRIORITY" ]]; then
    PRIORITYOVERLAY="textoverlay text=\"$PRIORITY\" valignment=top halignment=left font-desc=\"Sans, 36\" !"
    ZN_PRIORITY="--priority $PRIORITY"
else
    PRIORITYOVERLAY=""
fi

ZN_EXPRESS=""
if [[ -n "$EXPRESS" ]]; then
    ZN_EXPRESS="--express"
    EXPRESSOVERLAY="textoverlay text=\"Express\" valignment=top halignment=right font-desc=\"Sans, 36\" !"
else
    EXPRESS="false"
    EXPRESSOVERLAY="textoverlay text=\"No Express\" valignment=top halignment=right font-desc=\"Sans, 36\" !"
fi


# GST_TRACERS="latency(flags=element)" GST_DEBUG=GST_TRACER:7 \
gst-launch-1.0 videotestsrc is-live=1 ! \
    video/x-raw,format=NV12,width=1280,height=720 ! \
    $PRIORITYOVERLAY \
    $EXPRESSOVERLAY \
    timeoverlay valignment=4 halignment=1 ! tee name=t \
    t. ! queue ! xvimagesink sync=0 \
    t. ! queue ! nvvideoconvert !  nvv4l2h265enc idrinterval=15 ! filesink location=/dev/stdout | \
    RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 --pub stream/$PRIORITY/$EXPRESS $ZN_PRIORITY $ZN_EXPRESS
