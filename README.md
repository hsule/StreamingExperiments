# Streaming Project

## Prerequisites
### On Client
Follow the setup instructions from:
- [NVIDIA DeepStream SDK Installation Guide](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_Installation.html#dgpu-setup-for-ubuntu)
- [zncat (Zenoh-based streaming client)](https://github.com/NEWSLabNTU/zncat)
- [Zenoh installation](https://github.com/eclipse-zenoh/zenoh?tab=readme-ov-file#how-to-install-it)

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