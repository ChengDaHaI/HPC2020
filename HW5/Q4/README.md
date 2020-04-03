# How to run the CUDA code

- Packets: CUDA 9.0 or above

- Make: `make`

- Run: `./Q4`

- Discovery command: `srun --pty --export=ALL --partition=gpu --gres=gpu:1 --tasks-per-node 1 --nodes 1 --mem=10Gb /bin/bash`

# Output of CUDA C code
-`elapsed time: 4723.672363`
-`Total number is 1000724`

