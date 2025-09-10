#!/bin/sh
gdb -ex r --args spawn-fcgi -a 127.0.0.1 -p 4008 -n treebird
