import numpy as np
from scipy.io import mmread
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s matrix.mtx partition.par.n" % sys.argv[0]
        sys.exit(1)

    mtx_file = sys.argv[1]
    partfile = sys.argv[2]
    n_procs = int(sys.argv[2][-1])

    print "Reading... ",
    sys.stdout.flush()
    A = mmread(mtx_file)
    A = np.maximum(A,A.transpose())
    A = A.tocsr()
    print "Done!"

    n_nodes = A.shape[0]
    n_block = n_nodes/n_procs
    
    partition = np.loadtxt(partfile, dtype=int)
    for i in range(n_procs):
        outfilename = partfile + '.%d' % (i) 
        out_file = open(outfilename,'w')
        nodes = np.where(partition==i)[0]
        buf = "\n".join(map(str, nodes )) + '\n'
        out_file.write(buf)
        _ , cols = A[nodes,:].nonzero()
        cols = np.unique(cols)
        for j in range(n_procs):
            if j == i:
                continue
            neighs = cols[partition[cols] ==j ]
            if len(neighs) >0:
                buf = 'remote %d\n' % (j)
                buf += '\n'.join(map(str,neighs)) + '\n'
                out_file.write(buf)
    out_file.close()

