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

tests = [("Test 5.1: mapping all the thingss", "FIFO", "test5.1.in", "test5.1.out"),
         ("Test 5.2: mapping one thing", "FIFO", "test5.2.in", "test5.2.out"),
         ("Test 5.3: LRU stress test", "LRU", "test5.3.in", "test5.3.out"),
         ("Test 5.4: Fibonacci", "FIFO", "test5.4.in", "test5.4.out"),
         ("Test 5.5: multi-process Fibonacci", "FIFO", "test5.5.in", "test5.5.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
