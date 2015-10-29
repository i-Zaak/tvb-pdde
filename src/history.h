#include<vector>
#include <boost/circular_buffer.hpp>
#include<iostream>

class history_buffers {
	std::vector<boost::circular_buffer<double> > buffers;
	public:
		history_buffers(std::vector<int> n_verts);
		double get_value(int node_id, int position);
		void add_value(int node_id, double value);
		void add_values(std::vector<double> values);
		friend std::ostream& operator<< (std::ostream &out, history_buffers &h_bufs);
};
