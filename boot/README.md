# INTRODUCTION
This is the base directory for bootloader development using Zephyr. Specifically, Zephyr is directly compatible and integrated with MCUBoot - [GitHub MCUBoot](https://github.com/mcu-tools/mcuboot/tree/main)

- "Hello world" skeleton application which can be used as a starting point for Zephyr application development using mcuboot. It includes the configuration "glue" needed to make the application loadable by mcuboot in addition to a basic Zephyr hello world application's code.

## Getting Started

- MCUBoot has an example project called `hello_world` which is a good place to start and can be found inside the zephyrproject package -> `~/zephyrproject//zephyrproject/bootloader/mcuboot/samples/zephyr/hello-world`  
- Also see the following pages for more context on MCUBoot's integration with Zephyr - [mcuboot/readme-zephyr](https://docs.mcuboot.com/readme-zephyr)
