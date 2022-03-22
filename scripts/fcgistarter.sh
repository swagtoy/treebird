#!/bin/bash
kill -9 $(pgrep treebird.cgi) > /dev/null 2>&1
fcgistarter -c $PWD/../dist/treebird.cgi -p 4000 -N 1
