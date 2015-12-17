from scipy.io import mmread
from scipy.sparse import triu
import numpy as np
import sys


def maximum (A, B):
    tmp = A - B
    tmp.data = np.where(tmp.data < 0, 1, 0)
    return A - A.multiply(tmp) + B.multiply(tmp)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print "Usage: %s matrix_in.mtx matrix_out.adj [float|int]" % sys.argv[0]
        sys.exit(1)
    # the weight could be also length of the buffer of particular connection
    if sys.argv[3] == 'int':
        metis_format = True
    else:
        metis_format = False
    print "Reading... ",
    sys.stdout.flush()
    A = mmread(sys.argv[1])    
    if metis_format:
        A = maximum(A,A.transpose())
    A = A.tocsr()
    print "Done!"

    out_f = open(sys.argv[2],'w')

    print "Writing... ",
    sys.stdout.flush()
    if metis_format:
        nnz = A.nnz/2
    else:
        nnz = A.nnz # we interpret the matrix as symmetric.
    header = "%d %d 001\n" %(A.shape[0], nnz)
    out_f.write(header)
    for i in xrange(A.shape[0]):
        if A.indptr[i]< A.indptr[i+1]:
            #line = "%d "%(i+1) 
            line = ""
            for j in xrange(A.indptr[i], A.indptr[i+1]):
                if metis_format:
                    line+= " %d %d" %(A.indices[j]+1,int(1/(A.data[j])*1000)) 
                else:
                    line+= " %d %f" %(A.indices[j]+1,A.data[j])
        line += "\n"
        out_f.write(line)
    print "Done!"



