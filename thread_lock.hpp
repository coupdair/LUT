#ifndef _THREAD_LOCK_
#define _THREAD_LOCK_

//C++ base
#include <iostream>
#include <string>

//OpenMP
#include <omp.h>

//! base lock (with OpenMP)
class CBaseOMPLock
{
public:
  std::string class_name;
  int id;
  int tn;
  bool debug;

  CBaseOMPLock(omp_lock_t* lock){debug=false;class_name="CBaseOMPLock";id=omp_get_thread_num();tn=omp_get_num_threads(); if(debug) printf("t%d/%d,class=%s\n",id,tn,class_name.c_str());/*warning*/if(1) printf("%p",(void*)lock);}
  virtual void unset_lock(){}
  virtual void print(const char* message, bool unset=true){printf("t%d/%d ",id,tn); if(debug) printf(",class=%s\n",class_name.c_str());printf(message /*warning*/,unset);}
};//CBaseOMPLock

//! print lock (with OpenMP)
class CPrintOMPLock: public CBaseOMPLock
{
public:
  omp_lock_t *p_print_lock;
  CPrintOMPLock(omp_lock_t* lock):CBaseOMPLock(lock){class_name="CPrintOMPLock"; p_print_lock=lock;}
  virtual void unset_lock(){omp_unset_lock(p_print_lock);}
  virtual void print(const char* message, bool unset=true)
  {//locked section
    omp_set_lock(p_print_lock);
    if(debug) printf("class=%s\n",class_name.c_str());
    printf("t%d/%d %s",id,tn,message);
    fflush(stdout);
    if(unset) omp_unset_lock(p_print_lock);
  }//print
};//CPrintOMPLock

//! access lock (with OpenMP)
class CAccessOMPLock: public CBaseOMPLock
{
public:
  omp_lock_t *p_access_lock;
  CAccessOMPLock(omp_lock_t* lock):CBaseOMPLock(lock){debug=true;class_name="CAccessOMPLock"; p_access_lock=lock;}
  virtual void unset_lock(){omp_unset_lock(p_access_lock);}
  virtual void wait_for_status(unsigned char &what, const int status, const int new_status, unsigned int &c)
  {
    unsigned char a=99;
    do
    {//waiting for status
      //locked section
      {
        omp_set_lock(p_access_lock);
        a=what;
        if(a==status) what=new_status;
        omp_unset_lock(p_access_lock);
      }//lock
      ++c;
    }while(a!=status);//waiting for free
  }//wait_for_status

  virtual void search_for_status(std::vector<unsigned char> &what, int &n
    , std::vector<unsigned int> &index, int &d
    , const int status, const int new_status, unsigned int &c)
  {
    unsigned char a=99;
    bool found=false;

    //locked section
    {
      omp_set_lock(p_access_lock);
      //search for status
      for(int i=n;i<what.size();++i)
      {//searching for status
        a=what[i];
        if(a==status) {what[i]=new_status;found=true;n=i;d=index[i];break;}
        ++c;
      }//for loop
      if(!found)
      for(int i=0;i<n;++i)
      {//searching for status
        a=what[i];
        if(a==status) {what[i]=new_status;found=true;n=i;d=index[i];break;}
        ++c;
      }//for loop
     //! \bug [high] should be in a loop until done_status (to add)
     if(!found) printf("info: %s/%s work not found\n",__FILE__,__func__);fflush(stdout);
      omp_unset_lock(p_access_lock);
    }//lock
  }//search_for_status


  virtual void set_status(unsigned char &what, int old_status, int status, /*info:*/ char me, unsigned int i, unsigned int n, unsigned int c)
  {//locked section
    omp_set_lock(p_access_lock);
    //debug
    if(what!=old_status)/*filling*/ {printf("error: code error, acces should be 0x%x i.e. Filling for buffer#%d (presently is is 0x%x)",old_status,n,what);omp_unset_lock(p_access_lock);exit(99);}
    what=status;//filled

    //! \todo [high] need print lock and move after "omp_unset_lock(p_access_lock);"
    if(debug)
    {
      printf("%c%d/%d 4 B%02d #%04d wait=%d\n",me,id,tn,n,i,c);fflush(stdout);
    }//debug

    omp_unset_lock(p_access_lock);

  }//set_status


};//CAccessOMPLock

#endif //_THREAD_LOCK_

