#include "random.h"

boost_prng::boost_prng(): 	b_gen(),
						b_normal_dist(0,1),
						b_normal(b_gen,b_normal_dist){};

void boost_prng::seed( unsigned int seed)
{
	this->b_gen.seed(seed);
	this->b_normal_dist.reset();
}

double boost_prng::normal( )
{
	return this->b_normal();
}

void boost_prng::fill_normal(std::vector<double>& out_v)
{
	//for( 	std::vector<double>::iterator = out_v.begin;
	//		it< out_v.end(); it++) {
	//	it = this->generator();
	//}
	for (size_t i = 0; i < out_v.size(); i++) {
		out_v[i] = this->b_normal();
	}
}
