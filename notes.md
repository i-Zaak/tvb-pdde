# variants

connectivity
* surface only
* regional only
* surface + regional

resources
* SMP
* distributed (MPI)
* SMP + MPI

* [ ] implementovat mpi bench (umoznujici regional + surface)
* [ ] rezervovat 16 uzlu: qsub -l walltime=1d -l mem=400mb -l scratch=400mb -l nodes=16:ppn=1:x86_64:cl_ida
* [ ] otestovat (profil) skalovani: pokud je tam nejaka komunikace, budeme se snazit ji prekryt vypoctem: a) regiony b) tiled surface

