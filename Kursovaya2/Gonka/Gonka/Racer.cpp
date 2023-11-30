#include <iostream>
#include "Racer.h"
#include "AllroadBoots.h"
#include "Camel.h"
#include "Centavr.h"
#include "FastCamel.h"
#include "Broom.h"
#include "Eagle.h"
#include "FlyCarpet.h"


Racer::Racer() {
    name = "";
	index = 0;
	time = 0;
}

void Racer::reg(int rg, int trassa) {

    Boots boot;
    Camel cml;
    Centavr centavr;
    FastCamel fscml;
    Broom broom;
    Eagle eagle;
    FlyCarpet flycp;      

	index = rg;
	switch (rg) {
    case 1: time = boot.time_count(trassa); break;
    case 2: time = broom.time_count(trassa); break;
    case 3: time = cml.time_count(trassa);  break;
    case 4: time = centavr.time_count(trassa); break;
    case 5: time = eagle.time_count(trassa); break;
    case 6: time = fscml.time_count(trassa); break;
    case 7: time = flycp.time_count(trassa); break;
	}    

    switch (rg) {
    case 1: name = boot.Get_name(); break;
    case 2: name = broom.Get_name(); break;
    case 3: name = cml.Get_name();  break;
    case 4: name = centavr.Get_name(); break;
    case 5: name = eagle.Get_name(); break;
    case 6: name = fscml.Get_name(); break;
    case 7: name = flycp.Get_name(); break;
    }
}

double Racer::Get_time() {
    return time;
}

std::string Racer::Get_name() {
    return name;
}