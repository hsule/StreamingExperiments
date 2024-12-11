RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 -s stream | \
GST_TRACERS="bitrate(flags=element)" GST_DEBUG=GST_TRACER:7 \
gst-launch-1.0 fdsrc ! \
    h265parse ! \
    nvv4l2decoder ! \
    nvvideoconvert ! \
    xvimagesink

# # 預設參數
# PRIORITY="Data"
# EXPRESS="false"

# # 解析參數
# while [[ "$#" -gt 0 ]]; do
#     case $1 in
#         --priority) PRIORITY="$2"; shift ;;
#         --express) EXPRESS="true" ;;
#         *) echo "未知參數: $1"; exit 1 ;;
#     esac
#     shift
# done

# RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 -s stream/$PRIORITY/$EXPRESS | \
# gst-launch-1.0 fdsrc ! \
#     h265parse ! \
#     nvv4l2decoder ! \
#     nvvideoconvert ! \
#     xvimagesink