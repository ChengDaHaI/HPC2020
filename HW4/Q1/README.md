# MPI implementation for computing pi using dartboards methods
 
- compile by makefile:
 `make`
- command line argument: the number of darts, for example
 `./Q1 10000` 
- Discovery srun command:
 `srun --pty --export=ALL --partition=debug --nodes=1 --tasks-per-node=4 mpirun --mca btl_base_warn_component_unused 0 Q1 1000000`
  where --tasks-per-node= N, N is the number of tasks.
