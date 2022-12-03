### Project completed 12/2/2022 by Evan Scales, Chris Moore, and Mario Misencik

# COMPILATION

### To compile the source code, run:
make compile

### To delete any compiled c++ executables, run: #
make clean_compiled

# USAGE

### To generate an input file (courtesy of Dr. O'Reilly's code), run:
./filegenerator <filename (no .txt)> <# of inserts> <#of deletes> <# of lookups> <# of threads>

### To run the program on a single input file, run:
./userinterface <test.txt> <test_out.txt> <0/1> <0/1>

(the first <0/1> is <Hashmap/BST>, the second <0/1> is <Don't test/Do Test vs. single-threaded version>)

### To compare an output file to its expected output, run:
./compare <test_out.txt> <test_expected.txt>

### To clean all .txt files (generated or otherwise), run:
make clean

# UNIT TESTING
## There are several included unit tests to test standard and edge cases.
### *NOTE: replace "_hash" with "_bst" to run the equivalent test on the BST.*

### To test a single threaded example, run:
make test1t_hash

### To test a two-threaded example, run:
make test2t_hash

### To test a four-threaded example, run:
make test4t_hash

### To test an eight-threaded example, run:
make test8t_hash

### To test a zero-threaded example (should default to 1 thread), run:
make test0t_hash

### To test a ridiculously-many-threaded example (should default to 32 threads), run:
make test64t_hash

### To test a large (2500 commands) file on four threads, run:
make testlarge_hash

### To test a very large (25000 commands) file on four threads, run:
make testverylarge_hash

### To test a massive (250000 commands) file on four threads, run:
make testmassive_hash

### To test an empty (0 commands) file on four threads, run:
make testempty_hash

# TESTING LARGE (>1MB) FILES
## Please keep in mind that the standard UserInterface test (setting the fourth argument to 1) takes a very long time on large files, since it takes the average of 10 single-threaded runs against 10 custom-threaded runs. To test large files without waiting several minutes, run these two commands instead:
./userinterface <gianttest.txt> <gianttest_out.txt> <0/1> 0

./compare gianttest_out.txt gianttest_expected.txt

# DATA STRUCTURES APPROACH

The first issue was reading in the commands that the data structure will execute. The input file is read, then it is read line by line to create an 
opsStruct for each command. Each opsStruct has a key, the operation to perform, the value, and result. The result variable was added to keep the general 
order of the executed operations. The run method in UserInterface will read the first command for the number of threads, then while there are still 
commands to execute a the specified number of threads will be created to run each operation. Each data structure has a runOperation method that will 
insert, delete, or lookup depending on the command and return the result. Finally inside the thread function the result will be set in the appropriate 
opsStruct. Finally to write the output it will simply go through all the opsStruct and print the result of each operation to the specified file.

# PERFORMANCE & TESTING CONCLUSION

To test the code, we first created a Compare function (present in Compare.cpp and UserInterface.cpp, slightly modified) to check for correctness. This was used in conjunction with Professor O'Reilly's file generator code which he provided the class with, to test output against its expected output. We also used C++'s std::chrono library to record execution time for each run. In the test that was built into the user interface, we combined these two tests (correctness and speed) to compare 10 runs of a single-threaded baseline to the multi-threaded input, so as to average the two performances and determine if our program scaled properly. As well as this, we implemented a myriad of unit tests to check both edge and standard cases as described above.    

### Overall, our tests showed that the multi-threaded performance was quite an improvement compared to single-threaded. BST multi-threaded scaling was slightly worse than the hash map , but not by much:

2-threaded tests ran an avg. of 50-55% faster on the hash map / 45-50% faster on the BST.

4-threaded tests ran an avg. of 100-110% faster on the hash map / 100-105% faster on the BST.

8-threaded tests ran an avg. of 55-60% faster on the hash map / 55-70% faster on the BST.

32-threaded tests ran an avg. of 40-50% faster on the hash map / 40-70% faster on the BST.

*4 threads seemed to be the sweet spot (i.e., where the increased performance wasn't yet overshadowed by overhead costs) but any more seemed to generally give diminishing returns.*

### Some four-threaded timings on input sizes (very much dependent on this machine's speed, will vary):

1,000 operations: 0.03s (hash map) / 0.032s (BST)

10,000 operations: 0.28s (hash map) / 0.29s (BST)

100,000 operations: 2.85s (hash map) / 2.91s (BST)

1,000,000 operations: 28.7s (hash map) / 29.2s (BST)

*With more operations, the performance scales essentially the same (linear decrease in performance). The hash map / BST disparity indicates that the hash map is roughly 2.5-10% faster as a whole - not a large difference in performance.*
