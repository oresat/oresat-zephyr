#!/usr/bin/env python3

import canopen
import os
import time

ZEPHYR_BASE = os.environ['ZEPHYR_BASE']
EDS = os.path.join(ZEPHYR_BASE, 'samples', 'modules', 'canopennode',
                'objdict', 'objdict.eds')

NODEID = 10

network = canopen.Network()

network.connect()

node = network.add_node(NODEID, EDS)

# Green indicator LED will flash slowly
node.nmt.state = 'STOPPED'
time.sleep(5)

# Green indicator LED will flash faster
node.nmt.state = 'PRE-OPERATIONAL'
time.sleep(5)

# Green indicator LED will be steady on
node.nmt.state = 'OPERATIONAL'
time.sleep(5)

# Node will reset communication
node.nmt.state = 'RESET COMMUNICATION'
node.nmt.wait_for_heartbeat()

# Node will reset
node.nmt.state = 'RESET'
node.nmt.wait_for_heartbeat()

network.disconnect()
