#! /bin/bash

NAME=wshell
WORKING_DIR=$(pwd)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [[ "$WORKING_DIR" != "$SCRIPT_DIR" ]]; then
  echo "Please rerun this script from: $SCRIPT_DIR"
  exit 1
fi

echo "PART 1:"
cp ./myalloc.h test-part1/
cd test-part1
./test_part1.sh $*
cd ..

echo "PART 2:"
cp ./myalloc.h test-part2/
cd test-part2
./test_part2.sh $*
cd ..

echo "PART 3:"
cp ./myalloc.h test-part3/
cd test-part3
./test_part3.sh $*
cd ..

echo "PART 4:"
cp ./myalloc.h test-part4/
cd test-part4
./test_part4.sh $*
cd ..

echo "PART 5:"
cp ./myalloc.h test-part5/
cd test-part5
./test_part5.sh $*
cd ..

