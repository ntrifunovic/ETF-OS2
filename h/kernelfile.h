// File: kernelfile.h

#pragma once

#include "fs.h"
#include "cluster.h"

#include <string>

using namespace std;

typedef unsigned long DataNum;

class KernelFile {
public:
  KernelFile(Partition*, Entry, KernelFS*, string);
  ~KernelFile();  //zatvaranje fajla

  char write(BytesCnt, char* buffer); 
  BytesCnt read(BytesCnt, char* buffer);
  char seek(BytesCnt);
  BytesCnt filePos();
  char eof();
  BytesCnt getFileSize();
  char truncate();

  KernelFS* fs;

  string fullName;
  Entry entry;
  BytesCnt pos; // pozicija u fajlu
  Cluster cluster; // indeksni klaster
  DataNum dataPos; // pozicija u indeksnom klasteru (na odgovarajuci data klaster)
};