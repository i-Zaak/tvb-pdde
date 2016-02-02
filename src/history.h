#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <boost/math/special_functions/hermite.hpp>
#include "common.h"


/**
 Buffers for single population described by multiple state variables.
 */
class history_buffers {
	protected:
		boost::circular_buffer< std::vector<double> > buffer;
		double dt;
		unsigned long n_vars;
	public:
		history_buffers(unsigned long length, double dt, unsigned long n_vars);
		history_buffers( const history_buffers& other );
		unsigned long get_length();
		virtual double get_value(double delay, unsigned long var_id)=0;
		local_state_type get_values(double delay);
		double get_value_at(unsigned long position, unsigned long var_id);
		local_state_type get_values_at(unsigned long position);
		void add_state( local_state_type state);
		friend std::ostream& operator<< (std::ostream &out, history_buffers &h_bufs);
};



class history_factory
{
	public:
		virtual history_buffers* create_history(unsigned long length, double dt, unsigned long n_vars)=0;
};


class lint_history: public history_buffers{
	double linear_interpolate( double y1, double y2, double mu);
	public:
		lint_history(unsigned long length, double dt, unsigned long n_vars):history_buffers(length, dt, n_vars){};
		lint_history( const lint_history& other ):history_buffers(other) { };
		double get_value(double delay, unsigned long var_id);
};

class lint_history_factory: public history_factory
{
	public:
		lint_history* create_history(unsigned long length, double dt, unsigned long n_vars);
};

#endif
