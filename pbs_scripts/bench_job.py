import uuid
import jinja2
import sys



template = """#!/bin/bash
#PBS -l walltime=1h
#PBS -l nodes=1:ppn={{n_proc}}
#PBS -l mem={{mem}}gb
#PBS -j oe
#PBS -m e

trap 'clean_scratch' TERM EXIT

module add openmpi
cp /storage/brno3-cerit/home/izaak/tvb-pdde/var/conn{{conn}}.adj $SCRATCHDIR || exit 1
cp /storage/brno3-cerit/home/izaak/tvb-pdde/var/conn{{conn}}.adj.part.{{n_proc}}.* $SCRATCHDIR || exit 1
cp /storage/brno3-cerit/home/izaak/tvb-pdde/var/reg_conn{{n_proc}}.adj $SCRATCHDIR || exit 1
cp /storage/brno3-cerit/home/izaak/tvb-pdde/var/reg_conn{{n_proc}}.adj.{{n_proc}}.* $SCRATCHDIR || exit 1
cp /storage/brno3-cerit/home/izaak/tvb-pdde/bin/mpi_bench $SCRATCHDIR || exit 2 
cd $SCRATCHDIR || exit 3 

time mpirun -n {{n_proc}} mpi_bench conn{{conn}}.adj conn{{conn}}.adj.part.{{n_proc}} 0.1 {{n_iters}}
time mpirun -n {{n_proc}} mpi_bench surf_conn{{conn}}.adj surf_conn{{conn}}.adj.{{n_proc}} reg_conn{{n_proc}}.adj reg_conn{{n_proc}}.adj.{{n_proc}} 0.1 {{n_iters}}

"""

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print "Usage: %s file_desc n_proc n_iters\n\nwhere file_desc is e.g. 500k in conn500k.adj" % sys.argv[0]
        sys.exit(1)
    conn = sys.argv[1]
    n_proc = int(sys.argv[2])
    n_iters = int(sys.argv[3])

    filename = 'bench_%s_%d_%d_%s.sh'% (conn, n_proc, n_iters, str(uuid.uuid4())[:8]);
    print filename

    tmpl = jinja2.Template(template)
    file_content = tmpl.render( n_proc = n_proc, conn=conn, n_iters=n_iters, mem=24)

    out_f = open(filename, 'w')
    out_f.write(file_content)
    out_f.close()
