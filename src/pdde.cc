#include<iostream>
#include<vector>
#include "history.h"

int main()
{
	std::vector<int> buf_lengths = std::vector<int>(3);
	buf_lengths[0] = 2;
	buf_lengths[1] = 4;
	buf_lengths[2] = 7;
	history_buffers h_bufs = history_buffers(buf_lengths);
	h_bufs.add_value(0,3.5);
	
	for (int i = 0; i < 8; i++) {
		std::vector<double> vals = std::vector<double>(3);
		std::fill( vals.begin(), vals.end(), double(i));
		h_bufs.add_values(vals);
	}

	std::cout << h_bufs << std::endl;
	return 0;
}
