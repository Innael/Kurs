#pragma once
#include <string>

class Ground {
protected:
	std::string name = "Наземный";
	double speed = 1.0;
	double stamina = 1.0;
	double rest = 1.0;

public:
	Ground();
	std::string Get_name();
};
