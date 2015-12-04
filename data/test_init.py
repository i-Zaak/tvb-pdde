import numpy as np
import scipy.io as sio
from  scipy.sparse import csr_matrix
import networkx as nx
import sys

if len(sys.argv) != 3:
    print "Usage:"
    print " * to create new test: %s -n matfile.mtx" % sys.argv[0]
    print " * get expected results: %s -e matfile.mtx" % sys.argv[0]
    sys.exit(0)
elif sys.argv[1] == "-n":
    g = nx.random_graphs.watts_strogatz_graph(20,3,0.05)
    adj_rand = csr_matrix(adj.multiply(np.random.rand(*adj.shape)))
    sio.mmwrite("test_init.mtx", adj_rand)
elif sys.argv[1] == "-e":
    adj_rand = sio.mmread("test_init.mtx")

dt = 0.2
buflengths = np.ceil(np.max(adj_rand.toarray(),0)/dt) + 1

print "The expected lengths of buffers for dt=%f" %dt
print buflengths.astype(np.int32)
