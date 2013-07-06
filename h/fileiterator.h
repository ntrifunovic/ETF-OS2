// File: fileiterator.h

#pragma once

#include "part.h"
#include "partlib.h"

class FileIterator {
public:
  FileIterator(Partition*);
  void operator ++(int);
  void operator << (char);
  void operator >> (char&);

  bool end();

private:
  ClusterNo clusterNo;
  EntryNo entryNo;
  Partition* part;
  bool _end;
};