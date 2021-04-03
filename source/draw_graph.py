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
    cnt = 0
    average_latency = 0

    while True:
        line = file.readline()
        if not line:
            break
        x, y = map(int, line.split('\t'))
        y /= 1000
        if len(txn_lifetime) == 0:
            txn_lifetime.append(x)
            cnt = 1
            average_latency = y
        elif txn_lifetime[-1] == x:
            cnt += 1
            average_latency += y
        else:
            txn_lifetime.append(x)
            latency.append(average_latency/cnt)
            cnt = 1
            average_latency = y
    latency.append(average_latency/cnt)
    
    plt.plot(txn_lifetime, latency)
    plt.show()

if __name__ == "__main__":
    main(sys.argv)