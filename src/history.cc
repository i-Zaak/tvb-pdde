#include "history.h"
#include "math.h"
#include <exception>

typedef std::vector<boost::circular_buffer<double> > circular_buffers;
typedef boost::circular_buffer<double> circular_buffer;

history_buffers::history_buffers( std::vector<int> buffer_lengths, double dt )
{
	this->buffers = circular_buffers();
	for( 	circular_buffers::size_type i = 0; 
			i < buffer_lengths.size(); i++ ){
		this->buffers.push_back(circular_buffer( buffer_lengths[i]));
	}
	this->dt = dt;
	this->timesteps = std::vector<int>(buffers.size(),0);
}

double history_buffers::get_value_at(int node_id, int position)
{
	return buffers[node_id][position];
}

void history_buffers::add_value(int node_id, double value)
{
	timesteps[node_id]++;
	buffers[node_id].push_front(value);
}

void history_buffers::add_values(std::vector<double> values)
{
	//This is ridiculous. Refactor in C11.
	for(std::vector<int>::size_type i = 0; i <timesteps.size(); i++)
	{
		timesteps[i] ++;
	}

	for(std::vector<int>::size_type i = 0; i < values.size(); i++) 
	{
		buffers[i].push_front(values[i]);
	}
}

std::ostream& operator<< (std::ostream &out,history_buffers &h_bufs )
{
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

	return out;
}

double lint_history::linear_interpolate( double y1, double y2, double mu)
{
	return(y1*(1-mu)+y2*mu);
}

double lint_history::get_value(int node_id, double time)
{
    double relative_t = timesteps[node_id] * dt - time;
	int i1 = floor(relative_t/dt);
	int i2 = i1+1;
	double mu = (relative_t-i1*dt)/dt;
	double y1 = get_value_at(node_id,i1);
	double y2 = get_value_at(node_id,i2);

	return linear_interpolate(y1,y2, mu);
}

//double hermite_history::get_value_t(int node_id, double time)
//{
//
//	int i1 = floor((timesteps[node_id] * dt - time)/dt);
//	int i0 = i1+1;
//	int i2 = i1-1;
//	int i3 = i1-2;
//	double y0 = history_buffers.get_value(i0);
//	double y1 = history_buffers.get_value(i1);
//	double y2 = history_buffers.get_value(i2);
//	double y3 = history_buffers.get_value(i3);
//	double mu = //????
//
//	//interpolate
//	return hermite_interpolate(y0,y1,y2,y3, mu);
//}
//
//double hermite_history::hermite_interpolate(	double y0, double y1, double y2, 
//											double y3, double mu, 
//											double tension=0, double bias=0)
//{
//	/**
//	 * Taken from http://paulbourke.net/miscellaneous/interpolation/
//	 *
//	 * The coefficients should be computed only once when new value is added.
//	 */
//	throw std::logic_error("Not implemented.");
//	double m0,m1,mu2,mu3;
//	double a0,a1,a2,a3;
//	
//	mu2 = mu * mu;
//	mu3 = mu2 * mu;
//	m0  = (y1-y0)*(1+bias)*(1-tension)/2;
//	m0 += (y2-y1)*(1-bias)*(1-tension)/2;
//	m1  = (y2-y1)*(1+bias)*(1-tension)/2;
//	m1 += (y3-y2)*(1-bias)*(1-tension)/2;
//	a0 =  2*mu3 - 3*mu2 + 1;
//	a1 =    mu3 - 2*mu2 + mu;
//	a2 =    mu3 -   mu2;
//	a3 = -2*mu3 + 3*mu2;
//	
//	return(a0*y1+a1*m0+a2*m1+a3*y2);
//}
