#include <iostream>
#include "Race.h"
#include "Racer.h"

void marafon(Racer* arr, int itter) {
	Racer temp;
	for (int j = 0; j < itter; ++j)
	for (int i = 0; i < j; ++i) {
		if (arr[i].Get_time() > arr[j].Get_time()) {
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}
}