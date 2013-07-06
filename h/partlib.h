// File: partlib.h

#pragma once

#include "fs.h"
#include "part.h"

#define NULL 0

// Br. ulaza u klasteru root dir-a (Dvostuko ulanacana lista)
#define MAX_ENTRY_IN_CLASTER ((ClusterSize-2*sizeof(ClusterNo))/sizeof(Entry))
#define MAX_CLUSTER_NO_IN_CLUSTER ((ClusterSize)/sizeof(ClusterNo))

typedef unsigned long EntryNo;
typedef unsigned long pClusterNo;

// Postavlja na 0
void clear(Partition* particija, ClusterNo klaster);

void writeClusterNo(Partition* particija, ClusterNo klaster, pClusterNo pos, ClusterNo val);
ClusterNo readClusterNo(Partition* particija, ClusterNo klaster, pClusterNo pos);

void writeEntry(Partition* particija, ClusterNo klaster, EntryNo pos, Entry val);
Entry readEntry(Partition* particija, ClusterNo klaster, EntryNo pos);

// Vraca sledeci klaster u povezanoj listi
inline ClusterNo sledeci(Partition* particija, ClusterNo klaster) {
  return readClusterNo(particija, klaster, 0);
}
