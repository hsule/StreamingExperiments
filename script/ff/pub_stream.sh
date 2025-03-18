ffmpeg -f v4l2 -hwaccel cuda -input_format mjpeg -framerate 30 -video_size 1280x720 -i /dev/video0 -tune zero_latency -vf "showinfo" -f mpegts - |  RUST_LOG=info zncat --pub -m client -e tcp/140.112.31.242:7447 wtf

