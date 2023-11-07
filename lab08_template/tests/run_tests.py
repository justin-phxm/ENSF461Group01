#!/usr/bin/env python3

import sys
import os

tests = [
    "the simplest case: 2 entries in the vector, both are 1",
    "evaluate 4 entries in the vector, all are 1",
    "evaluate 4 entries in the vector, each are different, in ascending order",
    "evaluate 16 entries in the vector, all are 1",
    "evaluate 16 entries in the vector, each are different, in ascending order",
    "evaluate 16 entries in the vector, each are different and randomized",
    "evaluate 32 entries in the vector, all are 1",
    "evaluate 32 entries in the vector, each are different, in ascending order",
    "evaluate 32 entries in the vector, each are different and randomized",
    "evaluate 4096 entries in the vector, each are different and randomized",
    "testing with 32K input vector",
    "testing with 2**22 sized input vector"
]

usage = "Usage: run_tests.py r <mode> <#threads> <test>, or run_test.py p <test>\n"

def run_test(testnum, mode, numthreads):
    sys.stdout.write("Running test " + str(testnum) + ": " + tests[testnum] + "... ")
    inf = "test" + str(testnum) + ".in"
    outf = "test" + str(testnum) + ".out"
    os.system("../prefixscan.out " + mode + " " + numthreads + " " + inf + " temp.txt > /dev/null 2> /dev/null")
    if os.system("diff temp.txt " + outf + " > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")
    os.system("rm temp.txt")

if len(sys.argv) < 2:
    sys.stderr.write(usage)
    exit(1)
elif sys.argv[1] not in ["p", "r"]:
    sys.stderr.write(usage)
    exit(1)
elif sys.argv[1] == "r" and len(sys.argv) != 5:
    sys.stderr.write(usage)
    exit(1)
elif sys.argv[1] == "p" and len(sys.argv) != 3:
    sys.stderr.write(usage)
    exit(1)

if sys.argv[1] == "p":
    if sys.argv[2] == "all":
        for i in range(len(tests)):
            print("Test %d: %s" % (i, tests[i]))
    else:
        testnum = int(sys.argv[2])
        if testnum < 0 or testnum >= len(tests):
            sys.stderr.write("Invalid test number\n")
            exit(1)
        else:
            print("Test %d: %s" % (testnum, tests[testnum]))
elif sys.argv[1] == "r":
    mode = sys.argv[2]
    numthreads = sys.argv[3]
    if sys.argv[4] == "all":
        for i in range(len(tests)):
            run_test(i, mode, numthreads)
    else:
        testnum = int(sys.argv[4])
        if testnum < 0 or testnum >= len(tests):
            sys.stderr.write("Invalid test number\n")
            exit(1)
        run_test(testnum, mode, numthreads)
else:
    sys.stderr.write("Invalid command %s\n" % sys.argv[1])
    exit(1)
        