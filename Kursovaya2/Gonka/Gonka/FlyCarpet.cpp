#include <iostream>
#include "FlyCarpet.h"

FlyCarpet::FlyCarpet() {
	name = "Êîâ¸ğ-ñàìîë¸ò";
	speed = 10.0;
	cruise = 0.0;
}

double FlyCarpet::time_count(int trassa) {
	double hours = 0, track = 0, crs = cruise;
	if (trassa >= 1000 && trassa < 5000) crs = 3;
	else if (trassa >= 5000 && trassa < 10000) crs = 10;
	else if (trassa >= 10000) crs = 5;

	if (trassa >= 1000) trassa -= ((trassa * crs)/100);
	
	for (int i = 0; i < 2;) {
		for (double j = 0; j < (10 * (100 - crs)) / speed; ++j) {
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
	}
	return hours;
}