import sys
import numpy as np
import matplotlib.pyplot as plt

def main(argv):
    if len(argv) == 1:
        print('ERROR : No File')
        return

    file = open(argv[1])

    txn_lifetime = []
    latency = []
    timestamp = 0

    while True:
        line = file.readline()
        if not line:
            break
        latency.append(float(line))
        timestamp += 1
        txn_lifetime.append(timestamp)
        
    txn_lifetime = np.array(txn_lifetime, dtype='float')
    txn_lifetime = (txn_lifetime / txn_lifetime[-1])*300
    
    plt.plot(txn_lifetime, latency)
    plt.show()

if __name__ == "__main__":
    main(sys.argv)