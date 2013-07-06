// File: slobodniklasteri.h

#pragma once

#include "part.h"

class SlobodniKlasteri {
public:
	static void dodaj(Partition* particija, ClusterNo klaster);
	static ClusterNo uzmi(Partition* particija);
};