// File: buffer.h

#pragma once

#include <queue>
#include <windows.h>

#define POOL_SIZE 100

// Popunjava ulaz na pozciji pos sa vrednoscu val
// !!! val je 32 bitan, tipa ClusterNo !!!
#define WRITE_BUFF_ClusterNo(buff, pos, val) (*((ClusterNo*)((char*)buff) + (pos))) = (val)
// Cita ulaz pos iz buffer-a buff
// !!! vrednost je 32 bitna, tipa ClusterNo !!!
#define READ_BUFF_ClusterNo(buff, pos) (*((ClusterNo*)((char*)buff) + (pos)))

// Upisuje val u ulaz pos iz buffer-a buff tipa Entry !!! sa offsetom od 2*32 bita (2 pokazivaca) !!!
#define WRITE_BUFF_Entry(buff, pos, val) (*(Entry*)(((char*)buff) + 2*sizeof(ClusterNo))) = (val)
// Cita ulaz pos iz buffer-a buff tipa Entry !!! sa offsetom od 2*32 bita  (2 pokazivaca) !!!
#define READ_BUFF_Entry(buff, pos) (*(Entry*)(((char*)buff) + 2*sizeof(ClusterNo)))

class Buffer {
public:
  Buffer();
  ~Buffer();
  // Ovaj operator vraca buffer enkapsuliran u objektu koji se prosledjuje
  operator char* ();
  
private:
  class Pool;

  char* buffer;

  static Pool& pool;
};

class Buffer::Pool {
public:
  Pool();
  ~Pool();
  char* get(); // uzmi
  void ret(char*); // vrati

private:
  std::queue<char*> pool;
  CRITICAL_SECTION protectPool; // stiti red pool
};

void clearBuffer(Buffer);
