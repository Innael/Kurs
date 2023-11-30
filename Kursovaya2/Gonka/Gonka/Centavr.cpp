#include <iostream>
#include "Centavr.h"

Centavr::Centavr() {
	name = "Кентавр";
	speed = 15;
	stamina = 8;
	rest = 2;
}

double Centavr::time_count(int trassa) {
	double hours = 0, track = 0, rst = rest;
	for (int i = 0; i < 2;) {
		for (int j = 0; j < stamina; ++j) {
			if ((static_cast<double>(trassa) - track) < speed) {
				hours += (static_cast<double>(trassa) - track) / speed;
				track += speed;
			}
			else {
				track += speed;
				++hours;
			}
			if (track >= trassa) {
				i = 2; break;
			}
		}
		if (i == 2) break;
		hours += rst;		
	}
	return hours;
}