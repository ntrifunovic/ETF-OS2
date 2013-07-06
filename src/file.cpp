// File: file.cpp

#include "file.h"

#include "kernelfile.h"

#include "kernelfs.h"

File::File() {
}

File::~File() {
  HANDLE mutex = myImpl->fs->mutex;
  wait(mutex);
  delete myImpl;
  signal(mutex);
}

char File::write(BytesCnt cnt, char* buffer) {
   wait(myImpl->fs->mutex);
   char ret = myImpl->write(cnt, buffer);
   signal(myImpl->fs->mutex);

   return ret;
}

BytesCnt File::read(BytesCnt cnt, char* buffer) { 
   wait(myImpl->fs->mutex);
   BytesCnt ret = myImpl->read(cnt, buffer);
   signal(myImpl->fs->mutex);

   return ret;
}

char File::seek(BytesCnt cnt) {
  wait(myImpl->fs->mutex);
   char ret = myImpl->seek(cnt);
   signal(myImpl->fs->mutex);

   return ret;
}

BytesCnt File::filePos() {
  //wait(myImpl->fs->mutex);
   BytesCnt ret = myImpl->filePos();
   //signal(myImpl->fs->mutex);

   return ret;
}

char File::eof() {
  //wait(myImpl->fs->mutex);
   char ret = myImpl->eof();
   //signal(myImpl->fs->mutex);

   return ret;
}

BytesCnt File::getFileSize() {
  //wait(myImpl->fs->mutex);
   BytesCnt ret = myImpl->getFileSize();
   //signal(myImpl->fs->mutex);

   return ret;
}

char File::truncate() {
  wait(myImpl->fs->mutex);
   char ret = myImpl->truncate();
   signal(myImpl->fs->mutex);

   return ret;
}