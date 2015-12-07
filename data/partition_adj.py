import numpy as np
from scipy.io import mmread
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s matrix.mtx n" % sys.argv[0]
        sys.exit(1)

    mtx_file = sys.argv[1]
    n_procs = int(sys.argv[2])

    print "Reading... ",
    sys.stdout.flush()
    A = mmread(mtx_file)
    A = A.tocsr()
    print "Done!"

    n_nodes = A.shape[0]
    n_block = n_nodes/n_procs
    
    
    for i in xrange(n_procs):
        outfilename = mtx_file + '.part%d' % (i) 
        out_file = open(outfilename,'w')
        start = i*n_block
        stop = i*n_block + n_block
        buf = "\n".join(map(str,range(start,stop) )) + '\n$neighbors\n'
        out_file.write(buf)
        _ , cols = A[start:stop,:].nonzero()
        cols = np.unique(cols)
        cols = cols[(cols < start)|(cols >= stop)]
        buf = '\n'.join(map(str,(np.unique(cols)))) + '\n'
        out_file.write(buf)

