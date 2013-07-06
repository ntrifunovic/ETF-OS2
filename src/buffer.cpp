// File: buffer.cpp

#include "buffer.h"

#include "part.h"

#include <memory.h>

Buffer::Buffer() {
  buffer = pool.get();
}

Buffer::~Buffer() {
  pool.ret(buffer);
}

Buffer::Pool& Buffer::pool = Buffer::Pool();

Buffer::Pool::Pool() {
  InitializeCriticalSection(&protectPool);

  //for(int i = 0; i < POOL_SIZE; i++)
  //  pool.push(new char[ClusterSize]);
}

Buffer::Pool::~Pool() {
  DeleteCriticalSection(&protectPool); 

  while(!pool.empty())
    delete[] pool.front(), pool.pop();
}

char* Buffer::Pool::get() {
  char* ret;
  
  // ###
  EnterCriticalSection(&protectPool);
  
  if(pool.empty()) {
    ret = new char[ClusterSize];
  } else {
    ret = pool.front();
    pool.pop();
  }

  // ###
  LeaveCriticalSection(&protectPool);
  
  return ret;
}

void Buffer::Pool::ret(char *buffer) {
  // ###
  EnterCriticalSection(&protectPool);
  
  pool.size() < POOL_SIZE ? pool.push(buffer) : delete[] buffer;

  // ###
  LeaveCriticalSection(&protectPool);
}

Buffer::operator char* () {
  return buffer;
}

void clearBuffer(Buffer buff) {
  memset(buff, 0, ClusterSize);
}
