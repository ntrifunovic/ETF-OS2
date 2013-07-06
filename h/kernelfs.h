// File: kernelfs.h

#pragma once

#include "part.h"
#include "fs.h"

#include <windows.h>
#include <set>
#include <map>
#include <list>
#include <string>

using namespace std;

#define signal(x) ReleaseSemaphore(x,1,NULL)
#define wait(x) WaitForSingleObject(x,INFINITE)

#define NULL 0

#define MAX_NUM_OF_PARTITIONS 26

// konverzije iz char-a u indeks i iz indeksa u char
#define TO_CHAR(x) ((x) + 'A')
#define FROM_CHAR(x) ((x) - 'A')

typedef pair<pair<DWORD, string>, HANDLE> BankarInfo;

class KernelFS {
public:
  KernelFS();
  ~KernelFS();

  char mount(Partition* partition); 
  char unmount(char part);
  char format(char part);
  char readRootDir(char part, EntryNum n, Directory &d); 
  char doesExist(char* fname);
  char declare(char* fname, int mode); 
  File* open(char* fname);
  char deleteFile(char* fname);
  
  HANDLE mutex;
  list< BankarInfo > bankar_queue;

  
  bool bankarMoze(DWORD pid, string fname);
  bool bankarMoze(string fname);

  Partition* partitions[MAX_NUM_OF_PARTITIONS];

  bool canOpen[MAX_NUM_OF_PARTITIONS];
  int declaredFilesCount[MAX_NUM_OF_PARTITIONS]; 

  map<DWORD, set<string> > declared;
  map<DWORD, set<string> > opened;
};