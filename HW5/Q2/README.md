# How to run the CUDA code

- Packets: CUDA 9.0 or above

- Make: `make`

- Discovery command: `srun --pty --export=ALL --partition=gpu --gres=gpu:1 --tasks-per-node 1 --nodes 1 --mem=10Gb /bin/bash`

# Output of MPI C++ code

- The author index/indices that have the most co-author:
 `Author:   3336 has    343 co-authors`
- The elapsed time of the MPI code:
  `Time elapsed: 5.339682`

- The "output.txt" file where the distribution data is stored. Run `python3 plotDistribution.py` and get the "Distribution.pdf" graph. Numpy and Matplotlib are required.
