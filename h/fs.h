// File: fs.h

#pragma once

typedef unsigned long BytesCnt;
typedef unsigned long EntryNum;

const unsigned long ENTRYCNT=64; 
const unsigned int FNAMELEN=8;
const unsigned int FEXTLEN=3;

struct Entry {
  char name[FNAMELEN];
  char ext[FEXTLEN];
  char reserved;
  unsigned long firstIndexCluster;
  unsigned long size;
};

typedef Entry Directory[ENTRYCNT];

class KernelFS;
class Partition;
class File;

class FS {
public:
  ~FS();

  //montira particiju
  // vraca dodeljeno slovo
  // ili 0 u slucaju neuspeha
  static char mount(Partition* partition); 

  //demontira particiju oznacenu datim
  // slovom vraca 0 u slucaju neuspeha ili 1 u slucaju uspeha
  static char unmount(char part);

  // formatira particiju sa datim slovom;
  // vraca 0 u slucaju neuspeha ili 1 u slucaju uspeha
  static char format(char part);

  //prvim argumentom se zadaje particija, drugim redni broj
  //validnog ulaza od kog se počinje čitanje,
  //treci argument je adresa na kojoj se smesta procitani niz ulaza 
  static char readRootDir(char part, EntryNum n, Directory &d); 
  
  //argument je naziv fajla zadat
  //apsolutnom putanjom       
  static char doesExist(char* fname);

  //prvi argument je naziv fajla zadat apsolutnom putanjom,
  //drugi mod: 
  // 1 – najava koriscenja, 0 – odjava koriscenja zadatog fajla 
  static char declare(char* fname, int mode); 

  static File* open(char* fname);     
  static char deleteFile(char* fname);

protected:
  FS();
  static KernelFS *myImpl;
};