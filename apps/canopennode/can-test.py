#!/usr/bin/env python3

import canopen
import os

ZEPHYR_BASE = os.environ['ZEPHYR_BASE']
EDS = os.path.join(ZEPHYR_BASE, 'samples', 'modules', 'canopennode',
                'objdict', 'objdict.eds')

NODEID = 10

network = canopen.Network()

network.connect()

node = network.add_node(NODEID, EDS)
name = node.sdo['Manufacturer device name']

print("Device name: '{}'".format(name.raw))

network.disconnect()
