# How to run the MPI C++ code

- Packets: gcc 7.2.0 or above, openmpi 3.1.2 or above

- Make: `make`

- Discovery command: `srun --pty --export=ALL --partition=debug --nodes=4 --tasks-per-node=1 --mem-per-cpu=10GB mpirun --mca btl_base_warn_component_unused 0 Q1`

# Output of MPI C++ code

- The author index/indices that have the most co-author:
 `Author:   3336 has    343 co-authors`
- The elapsed time of the MPI code:
  `Time elapsed: 0.239682`