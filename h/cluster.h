// File: cluster.h

#pragma once

#include "part.h"
#include "fs.h"

#include <memory.h>

#include <map>
#include <queue>

using namespace std;

const unsigned long RootDirClusterEntriesSize = (ClusterSize - 2*sizeof(ClusterNo))/sizeof(Entry);
const unsigned long IndexClusterDataSize = (ClusterSize - sizeof(ClusterNo))/sizeof(ClusterNo);

#define LRU_SIZE 10

union _Cluster{
    struct /*RootDirCluster*/ {
      ClusterNo next;

      union {
        ClusterNo prev;
        ClusterNo free; // Za prvi ulaz root dira
      };

      Entry entries[RootDirClusterEntriesSize];
    };

    struct /*IndexCluster*/ {
      ClusterNo next;
      ClusterNo data[IndexClusterDataSize];
    };
  };

struct Cluster {
  static map<pair<Partition*, ClusterNo>, pair<_Cluster*, int> > kes;
  static map<pair<Partition*, ClusterNo>, _Cluster*> lru;
  static queue<pair<Partition*, ClusterNo>> lru_queue;

  Cluster(Partition* _particija, ClusterNo _no) : particija(_particija), no(_no) {
    load();
  }

  Cluster(Cluster& c) : particija(c.particija), no(c.no) {
    load();
  }

  ~Cluster() {
    save();
  }

  void clear() {
    memset((char *)_, 0, ClusterSize);
  }

  void operator ++(int) {
	ClusterNo next = _->next;
	save();

    no = next;
    load();
  }

  void operator --(int) {
	ClusterNo prev = _->prev;
    save();

    no = prev;
    load();
  }

  Cluster& operator = (Cluster &c) {
   save();
   particija = c.particija;
   no = c.no;
   load();

   return c;
  }

  Partition* particija;
  ClusterNo no;
  _Cluster* _;

private:
  
  void load();
  void save();

};
