import os.path
import argparse
import subprocess
import numpy as np
import matplotlib.pyplot as plt

# Option For Experiment
# --workload : type of workload
#   1. skewed
#   2. uniform
#   3. both
# --rocksdb : experiment_target_system
#   1. origin
#   2. custom
#   3. both
parser = argparse.ArgumentParser()
parser.add_argument('--workload', type=str, default='both',
                    choices=['both', 'skewed', 'uniform'], 
                    help='type of workload')
parser.add_argument('--rocksdb', type=str, default='both',
                    choices=['both', 'origin', 'custom'], 
                    help='experiment target system')

def save_graph(txn_lifetime, latency, filename):
    """
    Save Experiment Result as Image

    visualize result as graph

    and save that graph as .png file

    filename should not have filename extension
    
    """
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

    y = np.zeros(txn_lifetime.size)
    for row, title in zip(latency, subplot_title):
        prev_y = np.copy(y)
        y += row
        plt.fill_between(txn_lifetime, prev_y, y, alpha=0.5, label=title)

    plt.rc('legend', fontsize='xx-small')
    plt.legend(loc='upper left')
    plt.show()
    plt.savefig('./result/'+filename+'.png')


def run_experiment(executive):
    """
    Run Test Program and Collect Experiment Result

    run given executive as subprocess
    
    collect its output from stdout
    
    and make some argument for further step(visualize result)
    """
    txn_lifetime = []
    latency = []
    timestamp = 0

    p = subprocess.Popen('./'+executive, shell=True)

    for line in p.stdout.readlines():
        record = list(map(float, line.split('\t')))
        latency.append(record)
        timestamp += 1
        txn_lifetime.append(timestamp)

    p.wait()

    txn_lifetime = np.array(txn_lifetime, dtype='float')
    txn_lifetime = (txn_lifetime / txn_lifetime[-1])*100
    latency = np.array(latency).T

    return txn_lifetime, latency


def main(workload, rocksdb):
    executive_filename = {
        ('uniform', 'origin'): 'test_with_origin_rocksdb',
        ('uniform', 'custom'): 'test_with_custom_rocksdb',
        ('skewed', 'origin'): 'test_with_origin_rocksdb',
        ('skewed', 'custom'): 'test_with_custom_rocksdb',
    }

    for (workload_, rocksdb_), executive in executive_filename.items():
        if (workload != 'both' and workload != workload_) or (rocksdb != 'both' and rocksdb != rocksdb_):
            continue
        
        if not os.path.isfile(executive):
            print('ERROR : {} does not exist'.format(executive))
            return
        
        print('Running Experiment [{}, {}]...'.format(workload_, rocksdb_))
        txn_lifetime, latency = run_experiment(executive)
        save_graph(txn_lifetime, latency, '{}_{}'.format(workload_, rocksdb_))

if __name__ == "__main__":
    argv = parser.parse_args()
    main(argv.workload, argv.rocksdb)
