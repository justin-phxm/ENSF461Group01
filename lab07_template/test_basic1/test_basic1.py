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

tests = [("Test 1.1: empty trace", "test1.1.in", "test1.1.out"),
         ("Test 1.2: define", "test1.2.in", "test1.2.out"),
         ("Test 1.3: double-defines", "test1.3.in", "test1.3.out"),
         ("Test 1.4: context switching", "test1.4.in", "test1.4.out"),
         ("Test 1.5: invalid context switch", "test1.5.in", "test1.5.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
