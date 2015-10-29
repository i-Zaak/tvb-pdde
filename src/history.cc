#include "history.h"
typedef std::vector<boost::circular_buffer<double> > circular_buffers;
typedef boost::circular_buffer<double> circular_buffer;

history_buffers::history_buffers( std::vector<int> buffer_lengths ){
	buffers = circular_buffers();
	for( 	circular_buffers::size_type i = 0; 
			i < buffer_lengths.size(); i++ ){
		buffers.push_back(circular_buffer( buffer_lengths[i]));
	}
}



double history_buffers::get_value(int node_id, int position){
	return buffers[node_id][position];
}

void history_buffers::add_value(int node_id, double value){
	buffers[node_id].push_back(value);
}

void history_buffers::add_values(std::vector<double> values){
	for (int i = 0; i < values.size(); i++) {
		buffers[i].push_back(values[i]);
	}
}

std::ostream& operator<< (std::ostream &out,history_buffers &h_bufs ){
	out << "{" << std::endl;
	for( 	circular_buffers::iterator buffer = h_bufs.buffers.begin(); 
			buffer != h_bufs.buffers.end(); buffer++ ){
		out << std::distance(h_bufs.buffers.begin(), buffer) << ": [ "; 

		for( 	circular_buffer::iterator it = buffer->begin(); 
				it != buffer->end(); it++ ){
			out << *it << " ";
		}

		out << "]" << std::endl; 
	}
	out << "}" << std::endl;

}

