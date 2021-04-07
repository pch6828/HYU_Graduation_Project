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
    total_latency = 0

    while True:
        line = file.readline()
        if not line:
            break
        line = line.strip()
        if line == '<<<':
            total_latency = 0
            timestamp += 1
        elif line == '>>>':
            txn_lifetime.append(timestamp)
            latency.append(total_latency)
        else:
            splited_line = line.strip().split()
            prev_word = None
            for word in splited_line:
                if prev_word == 'Median:':
                    total_latency += float(word)
                prev_word = word
    txn_lifetime = np.array(txn_lifetime, dtype='float')
    txn_lifetime = (txn_lifetime / txn_lifetime[-1])*300
    
    plt.plot(txn_lifetime, latency)
    plt.show()

if __name__ == "__main__":
    main(sys.argv)