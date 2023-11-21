#!/usr/bin/env python3

import sys
import os

def run_test(numthreads):
    sys.stdout.write("Running test: process dmtx/ directory... ")
    os.system("./analyzedmtx.out " + numthreads + " dmtx/ temp.txt")
    os.system("sort temp.txt > out.txt")
    os.system("rm temp.txt")
    if os.system("diff out.txt ref.txt > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")
    os.system("rm out.txt")

if len(sys.argv) != 2:
    sys.stderr.write("Usage: run_test.py <#processes>\n")
    exit(1)

run_test(sys.argv[1])