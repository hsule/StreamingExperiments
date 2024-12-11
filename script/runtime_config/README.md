# GStreamer Encoder Dynamic Configuration Tool

This tool is inspired by discussions on the [NVIDIA Developer Forum](https://forums.developer.nvidia.com/t/adaptive-change-bitrate-of-encoder-nvv4l2h264enc-in-runtime/300570), with modifications for real-time encoder configuration.


---

## Features

- **Dynamic Parameter Adjustments**:
  - **Bitrate (`br`)**: Adjust the encoder's bitrate in bits per second (bps).
  - **Constant Quality (`cq`)**: Modify the encoder's constant quality parameter (range: `0-51`).
  - **IDR Interval (`idr`)**: Set the frequency of IDR frames (in number of frames).

- **Real-time Input**: Update encoder settings dynamically via standard input.

- **Customizable Pipeline**: Built on a GStreamer pipeline featuring a test video source and H.265 encoding.

---

## Build

To compile the code, run the following command:

```bash
g++ -o test main.cpp `pkg-config --cflags --libs gstreamer-1.0 gstreamer-app-1.0`
```

## Run

Then run the program:

```bash
./test | RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 --pub stream
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

### Example Usage

1. **Update bitrate**:
   ```text
   br=4000000
   ```
   This sets the encoder bitrate to 4 Mbps.

2. **Adjust constant quality**:
   ```text
   cq=25
   ```
   This configures the encoder for higher compression and lower quality.

3. **Change IDR interval**:
   ```text
   idr=30
   ```
   This inserts an IDR frame every 30 frames.

---

## Example GStreamer Pipeline

The program runs a GStreamer pipeline for video processing. Below is the pipeline used:

```text
videotestsrc is-live=1 ! \
video/x-raw,format=NV12,width=1920,height=1080 ! \
timeoverlay valignment=4 halignment=1 ! queue ! \
nvvideoconvert ! nvv4l2h265enc name=video_enc ! \
filesink location=/dev/stdout
```