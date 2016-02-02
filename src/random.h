#ifndef RANDOM_H
#define RANDOM_H

#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

/*
 * Wraps the random number generation. Implementations are expected to be thread safe.
 */
class rng
{
	public:
		virtual void seed( unsigned int seed)=0;
		virtual double normal( )=0;
		virtual void fill_normal( std::vector<double>& out_v)=0;
		// TODO which other types of noise will be needed?
};


class boost_prng : public rng
{
	private:
		boost::mt19937 b_gen;
		boost::normal_distribution<> b_normal_dist;
		boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > b_normal;
	public:
		boost_prng();
		void seed( unsigned int seed);
		double normal( );
		void fill_normal(std::vector<double>& out_v);
};
#endif
