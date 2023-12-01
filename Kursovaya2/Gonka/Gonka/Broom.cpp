#include <iostream>
#include "Broom.h"

Broom::Broom() {
	name = "Метла";
	speed = 20.0;
	cruise = 0.0;
}

double Broom::time_count(int trassa) {
	double hours = 0, track = 0, crs = cruise;
	int c = 0;
	c = trassa / 1000;
	crs = c;
	trassa *= ((100 - crs) / 100);
	for (int i = 0; i < 2;) {
		for (double j = 0; j < 50; ++j) {
			if ((static_cast<double>(trassa) - track) < speed) {
				if (static_cast<double>(trassa) > track) {
					hours += (static_cast<double>(trassa) - track) / speed;
					track += speed;
				}
				else i = 2; break;
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
		if (trassa <= 0) break; 
	}
	return hours;
}
