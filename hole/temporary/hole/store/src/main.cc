//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Jan 21 14:02:17 2009 texane
// Last update Sat Jan 24 04:47:53 2009 texane
//



#include <cstdio>
#include <cstdlib>
#include "pstdint.h"
#include "LocalStore.hh"



static void hexdump(const uint8_t* data, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    {
      if (i && !(i % 8))
	::printf("\n[%08x]: ", i);

      ::printf("%02x", data[i]);
    }
}


static bool
printBlock(LocalStore*& store, const LocalStore::BlockIdT& blockId, void*)
{
  LocalStore::BlockSizeT blockSize;
  LocalStore::StatusT status;
  uint8_t* blockData;

  ::printf("[+]");

  for (LocalStore::BlockSizeT i = 0; i < LocalStore::blockIdSize; ++i)
    ::printf("%02x", blockId[i]);
  ::printf(": ");

  status = store->fetchBlock(blockId, (void*&)blockData, blockSize);
  if (!LocalStore::isSuccess(status))
    goto nextBlock;

  ::printf("%u, ", blockSize);

  hexdump(blockData, blockSize);

  delete[] blockData;

 nextBlock:

  ::printf("\n");

  // continue

  return true;
}


static const LocalStore::BlockIdT& generateBlockId()
{
  static uint8_t blockId[LocalStore::blockIdSize];

  for (size_t i = 0; i < LocalStore::blockIdSize; ++i)
    blockId[i] = (uint8_t)(rand() % 0xff);

  return blockId;
}


int main(int, char**)
{
  LocalStore::BlockHandleT blockHandle;

  int error = -1;

  ::srand(::getpid());

  std::string dirPath = SystemApi::Fs::translatePath("/tmp/infinit.store") ;

  // delete if exists

  LocalStore::deleteStore(dirPath);

  // create new store

#define TMP_STORE_QUOTA (50 * 1024 * 1024)

  LocalStore* store = LocalStore::makeStore(dirPath, TMP_STORE_QUOTA);
  if (store == NULL)
    return -1;

  // write new blocks

  for (int i = 0; i < 42; ++i)
    store->writeBlock(generateBlockId(), "foo", sizeof("foo") - 1);

  // print blocks
  
  store->foreachBlock(printBlock);

  // success

  error = 0;

  if (store != NULL)
    delete store;

  return error;
}
