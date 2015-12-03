from gmsh import Mesh
import scipy.spatial.distance as scidist
from scipy.sparse import csr_matrix
from scipy.io import mmwrite

mesh = Mesh()
mesh.read_msh("mesh500.msh")

verts = mesh.Verts[:,0:2]

dists = scidist.pdist(verts)
dists[dists > 10] = 0
t_dists  = scidist.squareform(dists)

distmat = csr_matrix(t_dists)

mmwrite("conn500.mtx",distmat)
