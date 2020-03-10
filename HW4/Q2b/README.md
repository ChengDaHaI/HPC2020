# MPI Histogram with Dataset Splitting 

## Program framework

- Use MPI_Scatter to split dataset to equal size (N/mpi_size)
- Do sub-dataset histogram on each node
- Use MPI_Reduce and MPI_Gather to combine the results(data values in each bin, number of data values in each bin) in root node

## How to run

`
make;
mpirun Q2 5 100 1
`

where the first argument *5* is the number of bin/class, the second argument *100* is the number of integers in dataset, the third argument *1* indicates to print data values in each class.

## Output of program