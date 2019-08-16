#ifndef _DATA_ACCESS_
#define _DATA_ACCESS_

//thread lock
#include "thread_lock.hpp"

//C++ STL container
#include <vector>

//! locks for data access
/**
 * this access lock class is embedding both print lock and data access lock.
 * The status or state of the buffer is defined in \c ACCESS_STATUS_OR_STATE to specify e.g. which thread is currently accessing data.
 * The locks may be checked using \c check_locks, this should be called in sub-classes or user classes.
**/
class CDataAccess
{
public:
  std::string class_name;
  bool debug;
  CPrintOMPLock  lprint;
  CAccessOMPLock laccess;
  enum ACCESS_STATUS_OR_STATE {STATUS_FREE=0x0,STATUS_FILLED=0x1,STATUS_RECEIVED=0x1,STATUS_PROCESSED=0x2, STATE_FILLING=0xF,STATE_RECEIVING=0xE, STATE_STORING=0x5,STATE_SENDING=0x6,STATE_PROCESSING=0x7};

  CDataAccess(std::vector<omp_lock_t*> &lock)
  : lprint(lock[0]), laccess(lock[1])
  {
    debug=false;
    class_name="CDataAccess";
  }//constructor
  virtual void check_locks(std::vector<omp_lock_t*> &lock)
  {
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 lock for %s class.",class_name.c_str());
      exit(99);
    }//error exit
  }//constructor
};//CDataGenerator

#endif //_DATA_ACCESS_

