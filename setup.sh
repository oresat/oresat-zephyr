#!/bin/bash

ZEPHYR_VENV=".venv"
WEST_WORKSPACE="workspace"

if [ ! -d ${ZEPHYR_VENV} ]; then
  python3 -m venv ${ZEPHYR_VENV}
fi

source ./${ZEPHYR_VENV}/bin/activate

pip install west
west init -m https://github.com/zephyrproject-rtos/zephyr --mr v3.7.0 ${WEST_WORKSPACE}
cd ${WEST_WORKSPACE}
west update
west zephyr-export
pip install -r ./zephyr/scripts/requirements.txt

export ZEPHYR_BASE=$(pwd)/zephyr
echo "export ZEPHYR_BASE=$(pwd)/zephyr" >> ~/.bashrc
echo "ZEPHYR_BASE=${ZEPHYR_BASE} appended to ~/.bashrc"
echo "run source .venv/bin/activate to start virtual environment"
