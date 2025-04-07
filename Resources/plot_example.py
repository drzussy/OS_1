# OS 24 EX1
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r"/cs/usr/danemirovski/Documents/OS/ex1/OS_1/Resources/output4.csv")
data = data.to_numpy()

l1_size = 32*1024
l2_size = 250*1024  
l3_size = 9*1024*1024 

page_table_eviction_threshold = 2415919104 

plt.plot(data[:, 0], data[:, 1], label="Random access")
plt.plot(data[:, 0], data[:, 2], label="Sequential access")
plt.xscale('log')
plt.yscale('log')

# Add vertical lines for cache sizes
plt.axvline(x=l1_size, label="L1 (32 KiB)", c='r')
plt.axvline(x=l2_size, label="L2 (250 KiB)", c='g') 
plt.axvline(x=l3_size, label="L3 (9 MiB)", c='brown') 

# Add vertical line for Page Table Eviction Threshold
plt.axvline(x=page_table_eviction_threshold, label="Page Table Eviction Threshold (2.25 GiB)", c='purple') 

plt.legend()
plt.title("Latency as a function of array size")
plt.ylabel("Latency (ns log scale)")
plt.xlabel("Bytes allocated (log scale)")
plt.savefig("graph3.png",dpi=300)
plt.show()
