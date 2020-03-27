import numpy as np
import matplotlib.pyplot as plt

filename = "./output.txt"
datafile = np.fromfile(filename, dtype=np.int32, count=343, sep='', offset=0)
# for i in range(datafile.size):
#     print("Data:", datafile[i])

plt.plot(range(datafile.size), np.log10(datafile + 1))
# plt.title("Distribution")
plt.xlabel("number of co-authors")
plt.ylabel("$\log_{10}(number\quad of\quad authors + 1)$")
plt.xlim((1, 343))
plt.savefig("Distribution.pdf")
plt.show()
