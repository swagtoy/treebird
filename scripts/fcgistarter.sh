#!/bin/bash
kill -9 $(pgrep treebird) > /dev/null 2>&1
fcgistarter -c $(dirname $(readlink -f $0))/../treebird -p 4000 -N 1
