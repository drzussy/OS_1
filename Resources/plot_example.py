# OS 24 EX1
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r"C:\path\to\output.csv")
data = data.to_numpy()

l1_size = 0
l2_size = 0
l3_size = 0

plt.plot(data[:, 0], data[:, 1], label="Random access")
plt.plot(data[:, 0], data[:, 2], label="Sequential access")
plt.xscale('log')
plt.yscale('log')
plt.axvline(x=l1_size, label="L1 (?? ?iB)", c='r')
plt.axvline(x=l2_size, label="L2 (?? ?iB)", c='g')
plt.axvline(x=l3_size, label="L3 (?? ?iB)", c='brown')
plt.legend()
plt.title("Latency as a function of array size")
plt.ylabel("Latency (ns log scale)")
plt.xlabel("Bytes allocated (log scale)")
plt.show()
