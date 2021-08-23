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
    subplot_title = [
        'GET_SNAPSHOT_TIME',
        'GET_FROM_MEMTABLE_TIME',
        'GET_FROM_LEVEL_1',
        'GET_FROM_LEVEL_2',
        'GET_FROM_LEVEL_3',
        'GET_FROM_LEVEL_4',
        'GET_FROM_LEVEL_5',
        'GET_POST_PROCESS_TIME'
    ]
    
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
    for row, title in zip(latency, subplot_title):
        prev_y = np.copy(y)
        y += row
        plt.fill_between(txn_lifetime, prev_y, y, alpha = 0.5, label = title)
    plt.rc('legend', fontsize='xx-small')
    plt.legend(loc='upper left')
    plt.show()

if __name__ == "__main__":
    main(sys.argv)