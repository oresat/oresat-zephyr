# oresat-zephyr

OreSat firmware projects using Zephyr RTOS.

## Setup

**NOTE:** This mostly rework of https://docs.zephyrproject.org/latest/develop/getting_started/index.html

### Install dependencies

- Arch Linux

    ```bash
    paru -S git cmake ninja gperf dtc wget \
      python python-dev python-pip python-setuptools python-wheel \
      xz file make gcc sdl2 openocd
    ```

- Debian Linux

    ```bash
    sudo apt install --no-install-recommends git cmake ninja-build gperf \
      ccache dfu-util device-tree-compiler wget \
      python3-dev python3-pip python3-setuptools python3-tk python3-wheel \
      xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 openocd
    ```

### Install Zephyr SDK

- Arch Linux

    ```bash
    paru -S zephyr-sdk-bin
    ```

- Everything else

    ```bash
    cd /opt
    wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_linux-x86_64.tar.xz
    tar xvf zephyr-sdk-0.16.8_linux-x86_64.tar.xz
    cd zephyr-sdk-0.16.8
    ./setup.sh
    ```

### Add udev rule:

```bash
sudo cp /opt/zephyr-sdk-0.16.8/sysroots/x86_64-pokysdk-linux/usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload
```

### Install west

```bash
pip install west
```

### Get the source code

```bash
west init -m https://github.com/oresat/oresat-zephyr --mr master zephyr-workspace
cd zephyr-workspace
west update
```

### Export Zephyr CMake package:

```bash
west zephyr-export
```

### Install the rest of Zephyr's Python dependencies

```bash
pip install -r zephyr/scripts/requirements.txt
```

### Install OreSat Python dependencies

```bash
pip install -r requirements.txt
```

### Test compile and flash

```bash
cd zephyr/samples/basic/blinky
west build -p always -b nucleo_f091rc .
west flash --runner openocd
```
