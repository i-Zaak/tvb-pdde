from scipy.io import mmread
from scipy.sparse import triu
import numpy as np
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s matrix_in.mtx matrix_out.adj" % sys.argv[0]
        sys.exit(1)

    print "Reading... ",
    sys.stdout.flush()
    A = mmread(sys.argv[1])    
    A = np.maximum(A,A.transpose())
    A = A.tocsr()
    #A = triu(A,format='csr')
    print "Done!"

    out_f = open(sys.argv[2],'w')

    print "Writing... ",
    sys.stdout.flush()
    header = "%d %d 001\n" %(A.shape[0], A.nnz/2)
    out_f.write(header)
    for i in xrange(A.shape[0]):
        if A.indptr[i]< A.indptr[i+1]:
            #line = "%d "%(i+1) 
            line = ""
            for j in xrange(A.indptr[i], A.indptr[i+1]):
                #line+= " %d %f" %(A.indices[j]+1,A.data[j])
                line+= " %d %d" %(A.indices[j]+1,int(1/(A.data[j])*1000)) #here the weight could be length of the buffer of particular connection
            line += "\n"
            out_f.write(line)
    print "Done!"



