#include <iostream>
#include "Camel.h"



Camel::Camel() {
	name = "Верблюд";
	speed = 10;
	stamina = 30;
	rest = 5;
}

double Camel::time_count(int trassa) {
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
		rst = 8;
	}
	return hours;
}