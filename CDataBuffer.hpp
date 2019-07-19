#ifndef _DATA_BUFFER_
#define _DATA_BUFFER_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataAccess.hpp"

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

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
  {
    std::cout<< __FILE__<<"/"<<__func__<<" empty, should be implemented in inherited class."<<std::endl<<std::flush;
  };//iteration

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

};//CDataBuffer

#endif //_DATA_BUFFER_

