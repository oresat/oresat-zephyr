#!/bin/bash
find . -name "*.[h/c]" -not -path "./common/canopennode_v4/CANopenNode/*" | xargs clang-format -i
