#include <iostream>
#include "AllroadBoots.h"



Boots::Boots() {
	    name = "Ботинки-вездеходы";
		speed = 6;
		stamina = 60;
		rest = 10;
}

double Boots::time_count(int trassa) {
	double hours = 0, track =0, rst = rest;			
	for (int i = 0; i < 2;) {
		for (int j = 0; j < stamina; ++j) {
			if ((static_cast<long double>(trassa) - track) < speed) {
				hours += (static_cast<long double>(trassa) - track) / speed;
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
		rst = 5;
	}
	return hours;
}
