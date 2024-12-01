# project_transposition

In order to execute the program project_transposition.c create, inside the cluster login node, a folder named project_folder within the file project_transposition.c and the file project.pbs.
Then, inside the folder send the execution command qsub project.pbs .

The .pbs file does on his own the execution of the program for each number of threads and N, the results are stored automatically in files named results_N_numberofthreads.txt .

For example to see the project_transposition.c results, executed with a size matrix N=1024 and 16 threads, the name file will be results_1024_16.txt .
