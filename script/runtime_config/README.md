# GStreamer Encoder Dynamic Configuration Tool

This tool is inspired by discussions on the [NVIDIA Developer Forum](https://forums.developer.nvidia.com/t/adaptive-change-bitrate-of-encoder-nvv4l2h264enc-in-runtime/300570), with modifications for real-time encoder configuration.


---

## Features

- **Dynamic Parameter Adjustments**:
  - Bitrate (`br`)
  - Constant Quality (`cq`)
  - IDR Interval (`idr`)
  - Resolution (`res`)

- **Real-time Input**: Update encoder settings dynamically via standard input.

- **Customizable Pipeline**: Built on a GStreamer pipeline featuring a test video source and H.265 encoding.

---

## Build

To compile the code, run the following command:

```bash
g++ -o test.out main.cpp `pkg-config --cflags --libs gstreamer-1.0 gstreamer-app-1.0`
```

## Run

Then run the program:

```bash
./test.out | RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 --pub stream
```

The program will start a video stream processing pipeline and allow dynamic bitrate adjustment through standard input.


---

## Usage

### Input Format

Provide input in the following format to update parameters dynamically:

```text
key=value
```

#### Available Parameters:
- **`br`**: Set bitrate (e.g., `br=5000000` for 5 Mbps).
- **`cq`**: Set constant quality (e.g., `cq=30`, valid range: `0-51`).
- **`idr`**: Set IDR interval (e.g., `idr=15`, representing every 15 frames).
- **`res`**: Set resolution of the video (e.g., `res=1920x1080`)
