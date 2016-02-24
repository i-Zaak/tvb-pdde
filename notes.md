# variants

connectivity
* surface only
* regional only
* surface + regional

resources
* SMP
* distributed (MPI)
* SMP + MPI

* [x] implementovat mpi bench (umoznujici regional + surface)
* [ ] vytvorit testovaci dataset (parametrizovany)
  * [x] vlastnosti globalni site v TVB
  * [x] nagenerovat site (max 16 regionu, 2k uzlu kazdy (celkem 32k?), 100 hran)
  * [ ] jake delays mezi regiony?
* [ ] rezervovat 16 uzlu: qsub -l walltime=1d -l mem=400mb -l scratch=400mb -l nodes=16:ppn=1:x86_64:cl_ida
* [ ] otestovat (profil) skalovani: pokud je tam nejaka komunikace, budeme se snazit ji prekryt vypoctem: a) regiony b) tiled surface

