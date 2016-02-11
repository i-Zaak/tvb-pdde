```
integrator::step
  for all nodes:
    compute new local state:
	  integration scheme:
	    calls to dfun (with possible offset e.g. RK):
		  evaluate coupling(s) (here comes the offset)
		  compute df and dg
	put the local state to the global
  update history with new global state (callback to aggregative connectivity here?)
```






# Where to cut:
* ~~src/history.h~~
  * [x] ~~novy interface `global_history` obalujici vektor lokalnich bufferu a poskytujici fci `push_to_history`~~
  * [x] ~~implementace `all-to-all` globalni historie (prime pridani na konkretni buffer)~~
  * [x] ~~implementace `scatter-gather`, pro kazdy region jeden buffer obsahujici prumer nebo sumu spocitanou behem push_to_history~~
* ~~src/integrator.cc:56 (`dfun_eval`)~~
  * [x] ~~`l_coupling` bude mit velikost podle poctu konektivit~~
* ~~src/integrator.cc:57 (`dfun_eval`)~~
  * [x] ~~iterovat pres `coupling`~~
  * [x] ~~rozsirit `integrator::connectivity` na vektor~~
  * [x] ~~rozsirit `integrator::history` na vektor~~
  * [x] ~~zapsat na spravne misto v `l_coupling`~~
* ~~src/integrator.cc:47 (`integrator::step`)~~
  * [x] ~~iterovat pres historie~~
* ~~src/integrator.cc:76 (`integrator::euler`)~~
  * [x] ~~historie pro integracni schema (vychozi bude asi odkaz na prvni historii v bufferu)~~
* ~~src/integrator.cc:106 (`integrator::euler_maruyama`)~~
  * [x] ~~historie pro integracni schema ~~
* ~~tests/*~~
  * [x] ~~prepsat...~~
* ~~src/integrator.cc:65 (`integrator::dfun_eval()`)~~
  * [x] ~~vyresit vazbu mezi `coupling`, `connectivity`, `history` a iteraci pres uzly~~
* ~~src/model.cc:48 (`generic_2d_oscillator::operator()`)~~
  * [x] ~~pridat c_1 pro globalni konektivitu~~
* ~~src/connectivity.h~~
  * [x] ~~implementovat nacitani regionalnich map: zkonstruuje konektivitu mezi regiony a mapy uzel-region, nebo region-uzel~~
* ~~tests/connectivity_tests.cc~~
  * [x] ~~otestovat nacitani regionalni connectivity~~
* ~~tests/history_tests.cc~~
  * [x] ~~scatter-gather testy~~
* src/integrator.cc:122 (`integrator::constant_initial_conditions`)
  * [x] ~~prepsat uplne~~
  * [x] ~~IC regionalnich map~~
  * [ ] opravit volani tests/integrator_tests.cc:34
  * [ ] volani v tests/integrator_tests.cc:102
  * [ ] volani v src/seq_bench.cc:33
  * [ ] volani v src/mpi_bench.cc:74
* tests/integrator_tests.cc
  * [x] ~~vytvorit testovaci data pro regionalni coupling~~
  * [ ] integrace se scatter-gather connectivitou
* src/seq_bench_regional.cc
  * [ ] napsat :)
* MPI integrator
* MPI integrator testy
  * [ ] opravit IC tests/mpi_integrator_tests.cc:38
  * [ ] opravit IC tests/mpi_integrator_tests.cc:112
