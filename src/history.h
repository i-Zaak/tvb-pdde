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

class global_history
{
	protected:
		std::vector< history_buffers* > history; 
	public:
		global_history(std::vector< history_buffers* > history):
			history(history){};
		//TODO remove; hopefully not needed
		//void push_state(std::size_t node, local_state_type state); 
		virtual void push_state(global_state_type global_state);
		virtual history_buffers *get_buffers(std::size_t node) const;
		virtual std::size_t local_node_id(std::size_t global_node_id){return global_node_id;};
		// number of all nodes in the system
		virtual std::size_t n_nodes(){return this->history.size();};
};

class scatter_gather_history: public global_history
{
	private:
		std::vector< std::vector< std::size_t > > region_nodes;
		std::vector< std::size_t > nodes_region;
	public:
		scatter_gather_history(
				std::vector< history_buffers* > history, 
				std::vector< std::vector< std::size_t > > region_nodes,
				std::vector< std::size_t > nodes_region):
			global_history(history),
			region_nodes(region_nodes),
			nodes_region(nodes_region){};
		/**
		 * node is global index node, the values are aggregated within region
		 * and pushed to single bufer shared for all nodes in region.
		 */
		void push_state(global_state_type global_state);
		history_buffers *get_buffers(std::size_t node);
		std::size_t local_node_id(std::size_t global_node_id);
		std::size_t n_nodes(){return this->nodes_region.size();};
};

class empty_history: public global_history
{
	public:
		empty_history()
		void push_state(global_state_type global_state);
}

#endif
