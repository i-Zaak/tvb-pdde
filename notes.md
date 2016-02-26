# variants

connectivity
* surface only
* regional only
* surface + regional

resources
* SMP
* distributed (MPI)
* SMP + MPI

* [x] ~~implementovat mpi bench (umoznujici regional + surface)~~
* [x] ~~vytvorit testovaci dataset~~
  * [x] ~~vlastnosti globalni site v TVB~~
  * [x] ~~nagenerovat site (max 16 regionu, 2k uzlu kazdy (celkem 32k?), 100 hran)~~
  * [x] ~~jake delays mezi regiony?~~
  * [x] ~~regionalni konektivita~~
* [x] otestovat lokalne
* [x] pbs skript
* [x] rezervovat 16 uzlu: qsub -l walltime=1d -l mem=400mb -l scratch=400mb -l nodes=16:ppn=1:x86_64:cl_ida
* [x] otestovat (profil) skalovani: pokud je tam nejaka komunikace, budeme se snazit ji prekryt vypoctem: a) regiony b) tiled surface
* [x] vypocist mozne zlepseni prekryvanim vypoctu a komunikace
* [x] podivat se na profil komunikace --  bud v profilu, nebo zakomentovat vypoceta nechat jenom posilat data (jako ze idelne vybalancovny vypocet a jeste dobre prekryty?)
  * [ ] implementovat netrivialni reg_scatter/gather
  * [ ] rozdelit oblasti na tretiny az poloviny
  * [ ] nebo OpenMP uvnitr oblasti (4 procesy)

