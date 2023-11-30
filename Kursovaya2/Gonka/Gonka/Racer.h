#pragma once
#include <string>

class Racer {

	std::string name = "";
	int index = 0;
	double time = 0;

public:
	Racer();

	void reg(int rg, int trassa);
	double Get_time();
	std::string Get_name();
};
