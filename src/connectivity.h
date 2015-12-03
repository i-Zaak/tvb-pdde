#ifndef H_CONNECTIVITY
#define H_CONNECTIVITY

#include "common.h"
#include <string>

struct connection
{
	int from;
	//int to;
	double delay;
	double weight;
};
global_connectivity_type connectivity_from_mtx(std::string filename);

#endif
