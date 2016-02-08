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
* src/history.h
  * [ ] novy interface `global_history` obalujici vektor lokalnich bufferu a poskytujici fci `push_to_history`
  * [ ] implementace `all-to-all` globalni historie (prime pridani na konkretni buffer)
  * [ ] implementace `scatter-gather`, pro kazdy region jeden buffer obsahujici prumer nebo sumu spocitanou behem push_to_history
* src/integrator.cc:56 (`dfun_eval`)
  * [ ] `l_coupling` bude mit velikost podle poctu konektivit
* src/integrator.cc:57 (`dfun_eval`)
  * [ ] iterovat pres `coupling`
  * [ ] rozsirit `integrator::connectivity` na vektor
  * [ ] rozsirit `integrator::history` na vektor
  * [ ] zapsat na spravne misto v `l_coupling`
* src/integrator.cc:47 (`integrator::step`)
  * [ ] iterovat pres historie
* src/integrator.cc:76 (`integrator::euler`)
  * [ ] historie pro integracni schema (vychozi bude asi odkaz na prvni historii v bufferu)
* src/integrator.cc:106 (`integrator::euler_maruyama`)
  * [ ] historie pro integracni schema 
* src/integrator.cc:122 (`integrator::constant_initial_conditions`)
  * [ ] prepsat uplne
* src/connectivity.h
  * [ ] implementovat nacitani regionalnich map: zkonstruuje konektivitu mezi regiony a mapy uzel-region, nebo region-uzel
* src/model.cc:48 (`generic_2d_oscillator::operator()`)
  * [ ] pridat c_1 pro globalni konektivitu
* src/seq_bench_regional.cc
  * [ ] napsat :)
* tests/*
  * prepsat...



