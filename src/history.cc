#include "history.h"
#include "math.h"
#include <exception>

typedef boost::circular_buffer< std::vector<double> > state_buffer;

history_buffers::history_buffers( unsigned long length, double dt, unsigned long n_vars )
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

unsigned long history_buffers::get_length()
{
	return this->buffer.size();
}

double history_buffers::get_value_at(unsigned long position,unsigned long var_id)
{
	return buffer[position][var_id];
}

local_state_type history_buffers::get_values(double delay)
{
	local_state_type state = local_state_type(this->n_vars, 0.0);
	for(unsigned long i = 0; i < this->n_vars; i++){
		state[i] = this->get_value(delay, i);
	}
	return state;
}

local_state_type history_buffers::get_values_at(unsigned long position)
{

	local_state_type state = local_state_type(this->n_vars, 0.0);
	for(unsigned long i = 0; i < this->n_vars; i++){
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

double lint_history::get_value(double delay,unsigned long var_id)
{
	unsigned long i1 = floor(delay/dt);
	double mu = (delay - i1*dt)/dt;
	if (mu < std::numeric_limits<double>::epsilon() ) {
		return get_value_at(i1, var_id);
	}else{
		unsigned long i2 = i1+1;
		double y1 = get_value_at(i1, var_id);
		double y2 = get_value_at(i2, var_id);

		return linear_interpolate(y1,y2, mu);
	}
}

lint_history* lint_history_factory::create_history(unsigned long length, double dt, unsigned long n_vars)
{
	return new lint_history(length, dt, n_vars);
}
