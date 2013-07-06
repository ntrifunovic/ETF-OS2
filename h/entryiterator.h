// File: entryiterator.h

#pragma once

#include "part.h"
#include "fs.h"

#include "cluster.h"

class EntryIterator {
public:
  EntryIterator(Cluster& cluster, EntryNum entryNo);

  void operator ++(int);
  void operator --(int);
  Entry& operator * ();

  bool empty();
  bool end();
  void clear();

  Cluster& cluster;
  EntryNum entryNo;
};

// Proverava da li su isti naziv i ekstenzija, !!! ne gleda particiju !!!
bool cmp(EntryIterator, char *);
