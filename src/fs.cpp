// File: fs.cpp

#include "fs.h"

#include "kernelfs.h"

KernelFS* FS::myImpl = new KernelFS();

char FS::mount(Partition* partition) {
   wait(myImpl->mutex);
   char ret = myImpl->mount(partition);
   signal(myImpl->mutex);

   return ret;
}

char FS::unmount(char part) {
   wait(myImpl->mutex);
   char ret = myImpl->unmount(part);
   signal(myImpl->mutex);

   return ret;
}

char FS::readRootDir(char part, EntryNum n, Directory &d) {
   wait(myImpl->mutex);
   char ret = myImpl->readRootDir(part, n, d);
   signal(myImpl->mutex);

   return ret;
}

char FS::doesExist(char* fname) {
   wait(myImpl->mutex);
   char ret = myImpl->doesExist(fname);
   signal(myImpl->mutex);

   return ret;
}

File* FS::open(char* fname) {
   wait(myImpl->mutex);
   File* ret = myImpl->open(fname);
   signal(myImpl->mutex);

   return ret;
}

char FS::deleteFile(char* fname) {
   wait(myImpl->mutex);
   char ret = myImpl->deleteFile(fname);
   signal(myImpl->mutex);

   return ret;
}

char FS::format(char part) {
   wait(myImpl->mutex);
   char ret = myImpl->format(part);
   signal(myImpl->mutex);

   return ret;
}

char FS::declare(char* fname, int mode) {
   wait(myImpl->mutex);
   char ret = myImpl->declare(fname, mode);
   signal(myImpl->mutex);

   return ret;
}

FS::FS() {}

FS::~FS() {}