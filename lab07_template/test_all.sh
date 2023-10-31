#!/bin/sh

echo "Running test batch 1: basic tests"
cd test_basic1
./test_basic1.py
cd ..
echo "Done!"

echo "Running test batch 2: instructions tests"
cd test_instructions2
./test_instructions2.py
cd ..
echo "Done!"

echo "Running test batch 3: mapping tests"
cd test_mapping3
./test_mapping3.py
cd ..
echo "Done!"

echo "Running test batch 4: inspection tests"
cd test_inspection4
./test_inspection4.py
cd ..
echo "Done!"

echo "Running test batch 5: complex tests"
cd test_complex5
./test_complex5.py
cd ..
echo "Done!"
