#include<vector>
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <boost/math/special_functions/hermite.hpp>

class history_buffers {
	protected:
		std::vector< boost::circular_buffer<double> > buffers;
		double dt;
		std::vector<int> timesteps;
	public:
		history_buffers(std::vector<int> n_verts, double dt);
		virtual double get_value(int node_id, double time)=0;
		double get_value_at(int node_id, int position);
		void add_value(int node_id, double value);
		void add_values(std::vector<double> values);
		friend std::ostream& operator<< (std::ostream &out, history_buffers &h_bufs);
};



class lint_history: public history_buffers{
	double linear_interpolate( double y1, double y2, double mu);
	public:
		lint_history(std::vector<int> n_verts, double dt):history_buffers(n_verts, dt){};
		double get_value(int node_id, double time);
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
