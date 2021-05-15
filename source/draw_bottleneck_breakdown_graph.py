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

    fig, ax = plt.subplots()
    while True:
        line = file.readline()
        if not line:
            break
        record = list(map(float, line.split('\t')))
        latency.append(record)
        timestamp += 1
        txn_lifetime.append(timestamp)
        
    txn_lifetime = np.array(txn_lifetime, dtype='float')
    txn_lifetime = (txn_lifetime / txn_lifetime[-1])*100
    latency = np.array(latency).T

    y = np.zeros(txn_lifetime.size)
    for row in latency:
        prev_y = np.copy(y)
        y += row
        ax.fill_between(txn_lifetime, prev_y, y, alpha = 0.5)
    plt.show()

if __name__ == "__main__":
    main(sys.argv)