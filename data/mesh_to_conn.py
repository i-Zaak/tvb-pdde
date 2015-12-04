from gmsh import Mesh
import scipy.spatial.distance as scidist
from scipy.sparse import csr_matrix
from scipy.io import mmwrite

import sys

if len(sys.argv) != 3:
    print "Usage: %s mesh_in.msh conn_out.mtx" % sys.argv[0]
    sys.exit(1)

mesh = Mesh()
mesh.read_msh(sys.argv[1])

verts = mesh.Verts[:,0:2]

dists = scidist.pdist(verts)
dists[dists > 10] = 0
t_dists  = scidist.squareform(dists)

distmat = csr_matrix(t_dists)

mmwrite(sys.argv[2],distmat)
