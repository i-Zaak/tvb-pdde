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
	public:
		history_buffers(int length, double dt);
		virtual double get_value(double delay, int var_id)=0;
		double get_value_at(int position, int var_id);
		//todo get_values_at
		//void add_value(int node_id, double value);
		void add_state( local_state_type state);
		friend std::ostream& operator<< (std::ostream &out, history_buffers &h_bufs);
};



class lint_history: public history_buffers{
	double linear_interpolate( double y1, double y2, double mu);
	public:
		lint_history(int n_vars, double dt):history_buffers(n_vars, dt){};
		double get_value(double delay, int var_id);
};

//class hermite_history: public history_buffers{
//	std::vector<int> timesteps;
//	double hermite_interpolate(	double y0, double y1,
//								double y2, double y3,
//								double mu, double tension,
//								double bias);
//	public:
//		interpolated_history(std::vector<int> n_verts, double dt);
//		double get_value(int node_id, double time);
//};
