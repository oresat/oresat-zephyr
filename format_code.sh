#!/bin/bash
# While the '-no path "..."' could be done with a .clang-format-ignore file,
# a .clang-format-ignore file will only be used by clang-format v18.0.1 or newer.
#
# This method supports older clang-format versions.
find . -wholename "*.[h/c]" -not -path "*/gen/*" -not -path "*/build/*" -not -path "*/CANopenNode/*" | xargs clang-format -i $@
