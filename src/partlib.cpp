// File: partlib.cpp

#include "partlib.h"

#include "buffer.h"

void clear(Partition* particija, ClusterNo klaster) {
  Buffer buffer;

  clearBuffer(buffer);
  particija->writeCluster(klaster, buffer);
}

void writeClusterNo(Partition* particija, ClusterNo klaster, pClusterNo pos, ClusterNo val) {
  Buffer buffer;

  // Citamo klaster
  particija->readCluster(klaster, buffer);

  // Update-ujemo trazeno polje
  WRITE_BUFF_ClusterNo(buffer, pos, val);

  // Zapisujemo modifikovan klaster na particiju
  particija->writeCluster(klaster, buffer);
}

ClusterNo readClusterNo(Partition* particija, ClusterNo klaster, pClusterNo pos) {
  Buffer buffer;

  particija->readCluster(klaster, buffer);
  return READ_BUFF_ClusterNo(buffer, pos);
}

void writeEntry(Partition* particija, ClusterNo klaster, EntryNo pos, Entry val) {
  Buffer buffer;

  // Citamo klaster
  particija->readCluster(klaster, buffer);

  // Update-ujemo trazeno polje
  WRITE_BUFF_Entry(buffer, pos, val);

  // Zapisujemo modifikovan klaster na particiju
  particija->writeCluster(klaster, buffer);
}

Entry readEntry(Partition* particija, ClusterNo klaster, EntryNo pos) {
  Buffer buffer;

  particija->readCluster(klaster, buffer);
  return READ_BUFF_Entry(buffer, pos);
}
