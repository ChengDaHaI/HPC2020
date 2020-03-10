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
23 Integers in Bin # 1 :
  28	  60	 173	 124	  68	 136	  23	  59	  70	 168	  12	  43	 199	  92	 171	  85	 125	  44	  88	 179	  61	  13	  95	
23 Integers in Bin # 2 :
 384	 336	 387	 363	 212	 369	 394	 230	 374	 325	 316	 371	 282	 306	 328	 337	 314	 368	 365	 277	 400	 369	 227	
20 Integers in Bin # 3 :
 493	 422	 541	 427	 568	 430	 531	 457	 422	 538	 414	 527	 506	 583	 546	 435	 585	 404	 587	 540	
15 Integers in Bin # 4 :
 778	 794	 650	 691	 764	 737	 783	 785	 730	 751	 789	 652	 755	 677	 740	
19 Integers in Bin # 5 :
 887	 916	 927	 863	 930	 803	 920	 981	 957	 874	 863	 997	 926	 847	 858	 896	 815	 809	 933