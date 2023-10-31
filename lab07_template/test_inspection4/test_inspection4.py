#!/usr/bin/python3

import sys
import os

def run_test(test_name, strategy, trace_file, output_file):
    sys.stdout.write("Running test " + test_name + "... ")
    os.system("../memsym.out " + strategy + " " + trace_file + " temp.txt > /dev/null 2> /dev/null")
    if os.system("diff temp.txt " + output_file + " > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")

tests = [("Test 4.1: inspecting registers", "FIFO", "test4.1.in", "test4.1.out"),
         ("Test 4.2: inspecting the page table", "FIFO", "test4.2.in", "test4.2.out"),
         ("Test 4.3: Inspecting physical Memory", "FIFO", "test4.3.in", "test4.3.out"),
         ("Test 4.4: FIFO TLB replacement strategy", "FIFO", "test4.4.in", "test4.4.out"),
         ("Test 4.5: LRU TLB replacement strategy", "LRU", "test4.5.in", "test4.5.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
