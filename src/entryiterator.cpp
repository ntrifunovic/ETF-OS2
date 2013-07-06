// File: entryiterator.cpp

#include "entryiterator.h"

EntryIterator::EntryIterator(Cluster& c, EntryNum eNo) : cluster(c), entryNo(eNo) {

}

Entry generateEmptyEntry () {
  Entry emptyEntry;
  memset(&emptyEntry, 0, sizeof(emptyEntry));
  return emptyEntry;
}

bool EntryIterator::empty() {
  static Entry emptyEntry =  generateEmptyEntry();

  return memcmp(&cluster._->entries[entryNo], &emptyEntry, sizeof(Entry)) == 0;
}

bool EntryIterator::end() {
  return entryNo + 1 == RootDirClusterEntriesSize && cluster._->next == 0;
}


void EntryIterator::clear() {
  cluster._->entries[entryNo] = generateEmptyEntry();
}

void EntryIterator::operator ++(int) {
  // Ne mozemo dalje ako nema vise klastera u listi
  if(end())
    return;

  entryNo++;

  if(entryNo == RootDirClusterEntriesSize) {
    cluster++;
    entryNo = 0;
  }
}

void EntryIterator::operator --(int) {
  entryNo--;

  if(entryNo < 0) {
    cluster--;
    entryNo += RootDirClusterEntriesSize;
  }
}

Entry& EntryIterator::operator * () {
  return cluster._->entries[entryNo];
}