#!/bin/bash

ZEPHYR_VENV=".venv"

if [ ! -d ${ZEPHYR_VENV} ]; then
  python3 -m venv ${ZEPHYR_VENV}
fi

source ./${ZEPHYR_VENV}/bin/activate

pip install west
west init
west update
west zephyr-export
pip install -r ./zephyr/scripts/requirements.txt

echo "run \"python3 -m venv ${ZEPHYR_VENV}\" to start virtual environment"
