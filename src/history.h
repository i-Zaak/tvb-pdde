#include<vector>
#include <boost/circular_buffer.hpp>
#include<iostream>
#include <boost/math/special_functions/hermite.hpp>

class history_buffers {
	protected:
		std::vector<boost::circular_buffer<double> > buffers;
	public:
		history_buffers(std::vector<int> n_verts);
		double get_value(int node_id, int position);
		void add_value(int node_id, double value);
		void add_values(std::vector<double> values);
		friend std::ostream& operator<< (std::ostream &out, history_buffers &h_bufs);
};

class interpolated_history: public history_buffers{
	double dt;
	std::vector<int> timesteps;
	double hermite_interpolate(	double y0, double y1,
								double y2, double y3,
								double mu, double tension,
								double bias);
	double linear_interpolate( double y1, double y2, double mu);
	public:
		interpolated_history(std::vector<int> n_verts, double dt);
		double get_value_t(int node_id, double time);
		void add_value(int node_id, double value);
		void add_values(std::vector<double> values);
};
