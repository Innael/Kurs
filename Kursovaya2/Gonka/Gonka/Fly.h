#pragma once
#include <string>

class Fly {
protected:
	std::string name = "Воздушный";
	double speed = 1.0;
	double cruise = 0.05;	

public:
	Fly();
	std::string Get_name();
};

