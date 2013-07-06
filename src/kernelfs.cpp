// File: kernelfs.cpp

#include "kernelfs.h"

#include "file.h"

#include "cluster.h"
#include "slobodniklasteri.h"
#include "entryiterator.h"
#include "kernelfile.h"

#include <string>
#include <vector>

using namespace std;

KernelFS::KernelFS() {
  for(int i = 0; i < MAX_NUM_OF_PARTITIONS; i++) {
    partitions[i] = NULL;
  }

  mutex = CreateSemaphore(NULL,1,32,NULL);
}

KernelFS::~KernelFS() { 
  CloseHandle(mutex);
}

char KernelFS::mount(Partition* partition) {
  for(int i = 0; i < MAX_NUM_OF_PARTITIONS; i++)
    if(partitions[i] == NULL) {
      partitions[i] = partition;
      canOpen[i] = true;
	  declaredFilesCount[i] = 0;

      return TO_CHAR(i);
    }

  return 0;
}

char KernelFS::unmount(char part) {
  while(declaredFilesCount[FROM_CHAR(part)] > 0) {
    signal(mutex);
	Sleep(10);
	wait(mutex);
	canOpen[FROM_CHAR(part)] = false;
  }

  partitions[FROM_CHAR(part)] = NULL;
 
  canOpen[FROM_CHAR(part)] = true;

  return 1;
}

char KernelFS::format(char part) {
  Partition* partition = partitions[FROM_CHAR(part)]; // da bi se izbeglo dvostruko indeksiranje

  if(partition == NULL)
	  return 0;

  while(declaredFilesCount[FROM_CHAR(part)] > 0) {
	signal(mutex);
	Sleep(10);
	wait(mutex);
	canOpen[FROM_CHAR(part)] = false;
  }

  // Nulti klaster
  {
    Cluster head(partition, 0);
    head.clear();
  }
  // Ulancana lista slobodnih klastera
  for(ClusterNo i = 1, numOfClusters = partition->getNumOfClusters(); i < numOfClusters; i++) {
    SlobodniKlasteri::dodaj(partition, i);
  }

  canOpen[FROM_CHAR(part)] = true;

  return 1;
}

char KernelFS::readRootDir(char part, EntryNum n, Directory &d) {
  // Nulti klaster
  Cluster rootDir(partitions[FROM_CHAR(part)], 0);

  while(n >= RootDirClusterEntriesSize) {
    if (rootDir._->next == 0)
      return 0;

    rootDir++;
    n -= RootDirClusterEntriesSize;
  }

  EntryIterator it(rootDir, n);

  int cnt = 0;
  
  while (!it.end() && !it.empty() && cnt < ENTRYCNT ) {
     d[cnt++] = *it;
     it++;
  }

  return it.end() || it.empty() ? cnt : ENTRYCNT + 1;
}

char KernelFS::doesExist(char* fname) {
  Cluster rootDir(partitions[FROM_CHAR(fname[0])], 0);
  EntryIterator it(rootDir, 0);

  string dest = string(fname + 3);
  string name = dest.substr(0, dest.find_last_of('.'));
  name.append(FNAMELEN - name.length(), ' ');
  string ext = dest.substr(dest.find_last_of('.') + 1);
  

  while (!it.end() && !it.empty()) {
    if(memcmp((*it).name, name.c_str(), FNAMELEN) == 0 && memcmp((*it).ext, ext.c_str(), FEXTLEN) == 0)
      return 1;

    it++;
  }

  return 0;
}

char KernelFS::declare(char* fname, int mode) {
  if(mode == 1) {
    declared[GetCurrentThreadId()].insert(string(fname));
	declaredFilesCount[FROM_CHAR(fname[0])]++;
  }
  else {
	declaredFilesCount[FROM_CHAR(fname[0])]--;
    declared[GetCurrentThreadId()].erase(string(fname));
  }

  return 1;
} 

bool KernelFS::bankarMoze(DWORD pid, string fname) {
  set<string> otvoreni;

  for(map<DWORD, set<string> >::iterator it = opened.begin(); it != opened.end(); ++it) {
    otvoreni.insert(it->second.begin(), it->second.end());
  }
  
  if(otvoreni.find(fname)!=otvoreni.end())
    return false; // Otvorila ga je druga nit

  set<DWORD> niti;

  for(map<DWORD, set<string> >::iterator it = declared.begin(); it != declared.end(); ++it) {
    niti.insert(it->first);
  }

  opened[pid].insert(string(fname));
  otvoreni.insert(fname);

  while(!niti.empty()) {
    set<DWORD>::iterator nit;
    for(nit = niti.begin(); nit != niti.end(); ++nit) {
      set<string>::iterator fajl;
      for(fajl = declared[*nit].begin(); fajl != declared[*nit].end() && ( otvoreni.find(*fajl)==otvoreni.end() || opened[*nit].find(*fajl)!=opened[*nit].end()); ++fajl);

      if(fajl == declared[*nit].end()) {
        for(fajl = declared[*nit].begin(); fajl != declared[*nit].end(); ++fajl) {
           otvoreni.erase(*fajl);
        }

        break;
      }
    }

    if(nit == niti.end()) {
      opened[pid].erase(string(fname));
      return false;
    } else {
      niti.erase(*nit);
    }
  }

  return true;
}

bool KernelFS::bankarMoze(string fname) {
  return bankarMoze(GetCurrentThreadId(), fname);
}

File* KernelFS::open(char* fname) {
  if(!canOpen[FROM_CHAR(fname[0])] || declared[GetCurrentThreadId()].find(fname)==declared[GetCurrentThreadId()].end())
	return NULL;

  if(!bankarMoze(fname)) {
    HANDLE semafor = CreateSemaphore(NULL,0,32,NULL);
	BankarInfo info = make_pair(make_pair(GetCurrentThreadId(), fname), semafor);
	bankar_queue.push_back(info);
	
	signal(mutex);
	wait(semafor);
	wait(mutex);
	bankar_queue.remove(info);
	CloseHandle(semafor);
  }

  Partition* partition = partitions[FROM_CHAR(fname[0])];

  Cluster rootDir(partition, 0);
  EntryIterator it(rootDir, 0);

  string dest = string(fname + 3);
  string name = dest.substr(0, dest.find_last_of('.'));
  name.append(FNAMELEN - name.length(),' '); 
  string ext = dest.substr(dest.find_last_of('.') + 1);

  while (!it.end() && !it.empty()) {
    if(memcmp((*it).name, name.c_str(), FNAMELEN) == 0 && memcmp((*it).ext, ext.c_str(), FEXTLEN) == 0) { 
       break;
    }

    it++;
  }

  if(it.end()) {
    rootDir._->next = SlobodniKlasteri::uzmi(partition);
    ClusterNo prev = rootDir.no;
    it++;
	rootDir._->prev = prev;
  }

  if(it.empty()) {
    memcpy((*it).name, name.c_str(), FNAMELEN);
    memcpy((*it).ext , ext.c_str() , FEXTLEN);

    (*it).firstIndexCluster = SlobodniKlasteri::uzmi(partition);
    
    Cluster firstIndexCluster(partition, (*it).firstIndexCluster);
    firstIndexCluster.clear();

    (*it).reserved = 0;
    (*it).size = 0;
  }

  File* file = new File();
  file->myImpl = new KernelFile(partition, *it, this, string(fname));

  return file;
}     

char KernelFS::deleteFile(char* fname) {
  Partition* partition = partitions[FROM_CHAR(fname[0])];

  if(partition == NULL)
	return 0;

  while(true) {
	  bool good = true;

	  for(map<DWORD, set<string> >::iterator it = declared.begin(); it != declared.end(); ++it)
		  if(it->first != GetCurrentThreadId() && it->second.find(fname) != it->second.end())
			good = false;

	  if(good)
		break;
	  
	  signal(mutex);
	  Sleep(10);
	  wait(mutex);

  }

  Cluster rootDir(partition, 0);
  EntryIterator it(rootDir, 0);

  string dest = string(fname + 3);
  string name = dest.substr(0, dest.find_last_of('.'));
  name.append(FNAMELEN - name.length(),' '); 
  string ext = dest.substr(dest.find_last_of('.') + 1);

  while (!it.end() && !it.empty()) {
    if(memcmp((*it).name, name.c_str(), FNAMELEN) == 0 && memcmp((*it).ext, ext.c_str(), FEXTLEN) == 0) { 
       break;
    }

    it++;
  }

  declare(fname, 0);

  // Nismo nasli fajl
  if(!(memcmp((*it).name, name.c_str(), FNAMELEN) == 0 && memcmp((*it).ext, ext.c_str(), FEXTLEN) == 0))
    return 1;

  // Brisemo fajl
  for(Cluster c(partition, (*it).firstIndexCluster); c.no != 0; c++) {
    for(int i = 0; i < IndexClusterDataSize && c._->data[i] != 0; i++) {
      SlobodniKlasteri::dodaj(c.particija, c._->data[i]);
    }
  }

  // Kompresija
  Cluster end(rootDir);
  EntryIterator eit(end, it.entryNo);

  while (!eit.end() && !eit.empty())
    eit++;

  if(eit.empty())
	eit--;

  (*it) = (*eit);
  eit.clear();

  // Ako je bio jedini u klasteru root dir skida se
  if(eit.entryNo == 0 && end.no != 0) {
	Cluster endPrev(end.particija, end._->prev);
	endPrev._->next = 0;

	SlobodniKlasteri::dodaj(partition, end.no);
	
  }

  return 1;
}
