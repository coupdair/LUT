#ifndef _DATA_BUFFER_
#define _DATA_BUFFER_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataAccess.hpp"

//! base class intended to circular buffer sub-classes
/**
 * this base class declares \c constructor, \c iteration and \c run functions for circular buffer with shared access.
 * It is implementing \c run functions, i.e. loop on circular buffer, so \c iteration should be defined in sub-classes.
 * Data is shared, so both circular access and lock to it should be provided (see parameters: \c images, \c access, \c lock).
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataBuffer: public CDataAccess
{
public:

  CDataBuffer(std::vector<omp_lock_t*> &lock)
  : CDataAccess(lock)
  {
    debug=true;
    class_name="CDataBuffer";
    check_locks(lock);
  }//constructor

  //! one iteration for any loop
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
  {
    std::cout<< __FILE__<<"/"<<__func__<<"(buffer="<<access.width()<<", buffer="<<images.size()<<", buffer="<<n<<", iter="<<i<<") empty, should be implemented in inherited class."<<std::endl<<std::flush;
  };//iteration

  //! run for loop
  virtual void run(CImg<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    int nbuffer=images.size();
    for(int n=0,i=0;i<count;++i,++n)
    {
      this->iteration(access,images, n,i);
      //circular buffer
      if(n==nbuffer-1) n=-1;
    }//vector loop
  }//run

  //! run for loop
  virtual void run_loop(CImg<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    int nbuffer=images.size();
    int i=0;
    for(int n=0;n<nbuffer;++n)
    {
      this->iteration(access,images, n,i);
      //circular buffer
      if(n==nbuffer-1) n=-1;
    }//vector loop
  }//run_loop

};//CDataBuffer

#endif //_DATA_BUFFER_

