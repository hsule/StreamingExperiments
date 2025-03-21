# Streaming Project

## Prerequisites
### On Client
Follow the setup instructions from:
- [NVIDIA DeepStream SDK Installation Guide](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_Installation.html#dgpu-setup-for-ubuntu)
- [zncat - Command Line Replay for Zenoh](https://github.com/NEWSLabNTU/zncat?tab=readme-ov-file#installation)
- [Zenoh installation](https://github.com/eclipse-zenoh/zenoh?tab=readme-ov-file#how-to-install-it)
- [gst-plugin-tslatency](https://github.com/jerry73204/gst-plugin-tslatency/tree/main)
  - Use [PR #1](https://github.com/jerry73204/gst-plugin-tslatency/pull/1) for jitter calculation
  - Copy the compiled plugin `libgsttslatency.so` from `target/<arch>/release/` to `/usr/lib/<arch>/gstreamer-1.0/` (e.g., `x86_64-unknown-linux-gnu`, `aarch64-unknown-linux-gnu`)


### On Zenoh Router
- [Zenoh installation](https://github.com/eclipse-zenoh/zenoh?tab=readme-ov-file#how-to-install-it)

## Usage

### Encoder Configuration Tool

Follow the instructions in [this tutorial](runtime_config/README.md)

### Running the System

1. Start a Zenoh router on `140.112.31.242`.
2. Open the Zenoh subscriber:
    ```sh
    ./script/gst_time_sub.sh
    ```
3. Start the publisher:
    ```sh
    ./runtime_config/test.out | RUST_LOG=info zncat -m client -e tcp/140.112.31.242:7447 --pub stream
    ```
###