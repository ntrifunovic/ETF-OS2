// File: kernelfile.cpp

#include "kernelfile.h"

#include "kernelfs.h"
#include "slobodniklasteri.h"
#include "entryiterator.h"

#include <windows.h>

#include <memory.h>

#include <algorithm>

using namespace std;

KernelFile::KernelFile(Partition* par, Entry _entry, KernelFS* _fs, string _fullName)
  : entry(_entry), cluster(par, _entry.firstIndexCluster), fs(_fs), fullName(_fullName), dataPos(0), pos(0) {

}

KernelFile::~KernelFile() {
  Cluster rootDir(cluster.particija, 0);
  EntryIterator it(rootDir, 0);

  while (!it.end() && !it.empty()) {
    if(memcmp((*it).name, entry.name, FNAMELEN) == 0 && memcmp((*it).ext, entry.ext, FEXTLEN) == 0)
      (*it).size = entry.size;

    it++;
  }

  fs->opened[GetCurrentThreadId()].erase(fullName);

  for(list< BankarInfo >::iterator it = fs->bankar_queue.begin(); it != fs->bankar_queue.end(); it++) {
	  if(fs->bankarMoze(it->first.first, it->first.second)) {
		  signal(it->second);
		  break;
	  }
  }
}

char KernelFile::write(BytesCnt cnt, char* buffer) {
  while(cnt > 0) {  
    if(cluster._->data[dataPos] == 0)
		cluster._->data[dataPos] = SlobodniKlasteri::uzmi(cluster.particija);

    Cluster data(cluster.particija, cluster._->data[dataPos]);

    BytesCnt clusterPos = pos%ClusterSize;

    BytesCnt cut = min(cnt, ClusterSize - clusterPos);
    memcpy((char*)data._ + clusterPos, buffer, cut);

    cnt -= cut;
    pos += cut;
	buffer += cut;

    if(pos%ClusterSize == 0) {
      dataPos++;

      if(dataPos == IndexClusterDataSize) {
		  if(cluster._->next == 0) 
			cluster._->next = SlobodniKlasteri::uzmi(cluster.particija);

        cluster++;
        dataPos = 0;
      }
    }
  }

  entry.size = max(entry.size, pos);
  
  return 1;
}

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer) {
   cnt = min(cnt, entry.size - pos);

   while(cnt > 0) {  
    Cluster data(cluster.particija, cluster._->data[dataPos]);

    BytesCnt clusterPos = pos%ClusterSize;
    BytesCnt cut = min(cnt, ClusterSize - clusterPos);

    memcpy(buffer, (char*)data._ + clusterPos, cut);

    cnt -= cut;
    pos += cut;
	buffer += cut;

    if(pos%ClusterSize == 0) {
      dataPos++;

      if(dataPos == IndexClusterDataSize) {
        cluster++;
        dataPos = 0;
      }
    }
  }

  return cnt;
}

char KernelFile::seek(BytesCnt cnt) {
  if(cnt > entry.size)
    return 0;

  pos = cnt;

  cluster = Cluster(cluster.particija, entry.firstIndexCluster);

  while(cnt > ClusterSize*IndexClusterDataSize) {
	cnt -= ClusterSize*IndexClusterDataSize;
    cluster++;
  }
  
  dataPos = 0;

  while(cnt > ClusterSize) {
	  cnt -= ClusterSize;
	  dataPos++;
  }

  return 1;
}

BytesCnt KernelFile::filePos() {
  return pos;
}

char KernelFile::eof() {
  return pos == entry.size ? 2 : 0;
}

BytesCnt KernelFile::getFileSize() {
  return entry.size;
}

char KernelFile::truncate() {
 entry.size = pos;
 
 for(int i = dataPos + 1; i < IndexClusterDataSize && cluster._->data[i] != 0; i++) {
   SlobodniKlasteri::dodaj(cluster.particija, cluster._->data[i]);
   cluster._->data[i] = 0;
 }

 for(Cluster c(cluster.particija, cluster._->next); c.no != 0; c++) {
  for(int i = 0; i < IndexClusterDataSize && c._->data[i] != 0; i++) {
    SlobodniKlasteri::dodaj(c.particija, c._->data[i]);
  }
 }

 cluster._->next = 0;

 return 1;
}