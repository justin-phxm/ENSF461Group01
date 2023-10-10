# Lab 5 project template

## Contents

* scheduler.c: template file to complete to implement the scheduler
* example_stcf.in: example input to test the STCF scheduler
* example_stcf_analysis.out: expected output when running `./scheduler.out 1 STCF example_stcf.in 10`
* example_rr.in: example input to test the RR scheduler
* example_rr_analysis.out: expected output when running `./scheduler.out 1 RR example_rr.in 2`
* example_lt.in: example input to test the LT scheduler
* example_lt_analysis.out: expected output when running `./scheduler.out 1 LT example_rr.in 10`


## Notes

The `scheduler.c` file already contains some useful functions pertaining the implementation of a linked list (as requested by the assignment), reading the content of a workload file, and prototypes of functions implementing the STCF scheduler and the STCF scheduler analysis. You will need to fill in those functions, and create additional ones to implement the RR and LT schedulers and their respective scheduler analyses. Please note that you will also need to add a `Makefile`; running `make` in your project directory should produce the `scheduler.out` executable.

## Note on testing

The test input is provided **for your convenience only**; when grading, we may run the schedulers on different inputs. If your code runs correctly on the provided inputs but fails on other ones, you will still lose points. You are encouraged to test your code with different inputs, and to write your own tests. You can use the example files as inspiration for those.

A quick way to test your program is to use the `diff` command. For example, to compare the output of your program with the expected output for the FIFO scheduler, you can run:

```
./scheduler.out 1 STCF example_stcf.in 10 > my_output_stcf.out
diff my_output_stcf.out example_stcf_analysis.out
```

If the `diff` command does not produce any output, it means that your program produced the expected output. Otherwise, the `diff` command will show you the differences between the two files. You can also use the `diff` command to compare the output of your program with the expected output for the FIFO scheduler analysis, and for the SJF scheduler and SJF scheduler analysis. **Be mindful that the LT scheduler employs randomness, so each run will lead to potentially different output unless you always seed the random number generator with the same value.**

If you find it convenient, you may decided to create a `test` Makefile target that runs the scheduler on all the provided inputs, and compares the output with the expected output. This is not required, but it may be useful to you.