#!/usr/bin/env bash
ffmpeg \
                 -hwaccel cuda \
                 -i '/home/leann/Downloads/file_example_MP4_1920_18MG.mp4' \
                 -tune zero_latency \
		        -f mpegts \
                 - \
| RUST_LOG=info zncat --pub -m client -e tcp/140.112.31.242:7447 wtf
