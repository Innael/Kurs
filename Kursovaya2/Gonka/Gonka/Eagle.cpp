#include <iostream>
#include "Eagle.h"

Eagle::Eagle() {
	name = "Îğ¸ë";
	speed = 8.0;
	cruise = 6.0;
}

double Eagle::time_count(int trassa) {
	double hours = 0, track = 0, crs = cruise;
	for (int i = 0; i < 2;) {
		trassa = (trassa * (100 - crs))/100;
		for (double j = 0;;) {
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