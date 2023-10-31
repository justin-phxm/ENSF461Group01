#!/usr/bin/python3

import sys
import os

def run_test(test_name, trace_file, output_file):
    sys.stdout.write("Running test " + test_name + "... ")
    os.system("../memsym.out FIFO " + trace_file + " temp.txt > /dev/null 2> /dev/null")
    if os.system("diff temp.txt " + output_file + " > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")

tests = [("Test 3.1: basic mapping", "test3.1.in", "test3.1.out"),
         ("Test 3.2: basic unmapping", "test3.2.in", "test3.2.out"),
         ("Test 3.3: store/load in mapped memory", "test3.3.in", "test3.3.out"),
         ("Test 3.4: load from unmapped memory", "test3.4.in", "test3.4.out"),
         ("Test 3.5: page shared between processes", "test3.5.in", "test3.5.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")