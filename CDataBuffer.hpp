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
  ACCESS_STATUS_OR_STATE wait_status;
  ACCESS_STATUS_OR_STATE  set_status;

  CDataBuffer(std::vector<omp_lock_t*> &lock
  , ACCESS_STATUS_OR_STATE wait_status=STATUS_FREE, ACCESS_STATUS_OR_STATE  set_status=STATUS_FILLED)
  : CDataAccess(lock)
  , wait_status(wait_status), set_status(set_status)
  {
//    debug=true;
    class_name="CDataBuffer";
    check_locks(lock);
  }//constructor

  //! one iteration for any loop
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
  {
    std::cout<< __FILE__<<"/"<<__func__<<"(buffer="<<access.width()<<", buffer="<<images.size()<<", n="<<n<<", iter="<<i<<") empty, should be implemented in inherited class."<<std::endl<<std::flush;
  };//iteration
  //! one iteration for any loop
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    std::cout<< __FILE__<<"/"<<__func__<<"(buffer="<<access.width()<<", buffer="<<images.size()<<", buffer="<<accessR.width()<<", buffer="<<results.size()<<", n="<<n<<", iter="<<i<<") empty, should be implemented in inherited class."<<std::endl<<std::flush;
  };//iteration

  //! run for loop
  virtual void run(CImg<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    unsigned int nbuffer=images.size();
    for(unsigned int n=0,i=0;i<count;++i,++n)
    {
      this->iteration(access,images, n,i);
      //circular buffer
      if(n==nbuffer-1) n=-1;
     }//vector loop
  }//run

  //! run for loop
  template<typename Tdout>
  virtual void run(CImg<Taccess> &access,CImgList<Tdata> &images,  CImg<Taccess> &accessR,CImgList<Tdout> &results, unsigned int count)
  {
    unsigned int nbuffer=images.size();
    for(unsigned int n=0,i=0;i<count;++i,++n)
    {
      this->iteration(access,images, accessR,results, n,i);
      //circular buffer
      if(n==nbuffer-1) n=-1;
     }//vector loop
  }//run

};//CDataBuffer

#endif //_DATA_BUFFER_

