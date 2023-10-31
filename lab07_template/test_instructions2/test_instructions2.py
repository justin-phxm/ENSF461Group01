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

tests = [("Test 2.1: load immediate into register", "test2.1.in", "test2.1.out"),
         ("Test 2.2: load immediate into invalid register", "test2.2.in", "test2.2.out"),
         ("Test 2.3: instruction without define", "test2.3.in", "test2.3.out"),
         ("Test 2.4: add", "test2.4.in", "test2.4.out"),
         ("Test 2.5: add with context switch", "test2.5.in", "test2.5.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")