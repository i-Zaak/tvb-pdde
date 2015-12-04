#include "history.h"
#include "math.h"
#include <exception>

typedef boost::circular_buffer< std::vector<double> > state_buffer;

history_buffers::history_buffers( int length, double dt, unsigned int n_vars )
{
	this->buffer = state_buffer(length);
	this->dt = dt;
	this->n_vars = n_vars;
}

history_buffers::history_buffers( const history_buffers& other )
{
	this->buffer = state_buffer(other.buffer);
	this->dt = other.dt;
	this->n_vars = other.n_vars;
}

int history_buffers::get_length()
{
	return this->buffer.size();
}

double history_buffers::get_value_at(int position,int var_id)
{
	return buffer[position][var_id];
}

local_state_type history_buffers::get_values(double delay)
{
	local_state_type state = local_state_type(this->n_vars, 0.0);
	for(unsigned int i = 0; i < this->n_vars; i++){
		state[i] = this->get_value(delay, i);
	}
	return state;
}

local_state_type history_buffers::get_values_at(int position)
{

	local_state_type state = local_state_type(this->n_vars, 0.0);
	for(unsigned int i = 0; i < this->n_vars; i++){
		state[i] = this->get_value_at(position, i);
	}
	return state;
}

void history_buffers::add_state(local_state_type state)
{
	assert(state.size() == this->n_vars);
	buffer.push_front(state);
}

std::ostream& operator<< (std::ostream &out,history_buffers &h_bufs )
{
	out << "{" << std::endl;
	for( 	state_buffer::iterator buffer = h_bufs.buffer.begin(); 
			buffer != h_bufs.buffer.end(); buffer++ ){
		out << std::distance(h_bufs.buffer.begin(), buffer) << ": [ "; 

		for( 	local_state_type::iterator it = buffer->begin(); 
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

double lint_history::get_value(double delay,int var_id)
{
	int i1 = floor(delay/dt);
	double mu = (delay - i1*dt)/dt;
	if (mu < std::numeric_limits<double>::epsilon() ) {
		return get_value_at(i1, var_id);
	}else{
		int i2 = i1+1;
		double y1 = get_value_at(i1, var_id);
		double y2 = get_value_at(i2, var_id);

		return linear_interpolate(y1,y2, mu);
	}
}

lint_history* lint_history_factory::create_history(int length, double dt, unsigned int n_vars)
{
	return new lint_history(length, dt, n_vars);
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
