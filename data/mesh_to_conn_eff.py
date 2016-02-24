from gmsh import Mesh
import scipy.spatial.distance as scidist
from scipy.sparse import dok_matrix
from scipy.io import mmwrite
import numpy as np

import sys

def mesh_to_sparse_dist(mesh, d_max, out_fname):
    out_f = open(out_fname,'w')
    verts = mesh.Verts[:,0:2]
    x = verts[:,0]
    y = verts[:,1]

    x_inds = np.argsort(x)
    y_inds = np.argsort(y)

    n_nodes = len(x)
    #distmat = dok_matrix( (n_nodes,n_nodes) )
    print 'Processing [                     ]',
    print '\b'*23,
    nnz =0
    for n_id in xrange(len(verts)):
        if n_id%(n_nodes/20) ==0:
            print '\b.',
            sys.stdout.flush()
        x_n, y_n = verts[n_id,0:2]

        x_min, x_max = [x_n - d_max, x_n +d_max]
        y_min, y_max = [y_n - d_max, y_n +d_max]
        
        xsort_min = np.searchsorted(x,x_min,side='left',sorter=x_inds)
        xsort_max = np.searchsorted(x,x_max,side='right',sorter=x_inds)
        ysort_min = np.searchsorted(y,y_min,side='left',sorter=y_inds)
        ysort_max = np.searchsorted(y,y_max,side='right',sorter=y_inds)
        
        xids = x_inds[xsort_min:xsort_max]
        yids = y_inds[ysort_min:ysort_max]
        
        ids = np.intersect1d(xids,yids,assume_unique=True)

        dists = np.linalg.norm(verts[ids,:]-[x_n,y_n],axis=1)
        dists_th = dists < 6 #mm
        ids = ids[dists_th]
        dists = dists[dists_th]
        

        block = ''
        for i,m_id in enumerate(ids):
            if n_id !=m_id:
                block += '%d %d %f\n%d %d %f\n' % (n_id+1,m_id+1,dists[i],m_id+1,n_id+1,dists[i])
                nnz += 2
            #distmat[n_id,m_id] = dists[i]
            #distmat[m_id,n_id] = dists[i]
        out_f.write(block)

    out_f.close()
    print '\b]  Done!'
    #return distmat
    return len(verts), len(verts), nnz

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s mesh_in.msh conn_out.mtx" % sys.argv[0]
        sys.exit(1)
    d_max = 1.0
    mesh = Mesh()
    print "Reading... ",
    sys.stdout.flush()
    mesh.read_msh(sys.argv[1])
    print "Done!"
    n,m,nnz = mesh_to_sparse_dist(mesh, d_max, sys.argv[2] )
    #print "Writing... ",
    #sys.stdout.flush()
    #mmwrite(sys.argv[2],distmat)
    #print "Done!"
    print """
%%MatrixMarket matrix coordinate real general
%
    """
    print "%d %d %d" %(n,m,nnz)
