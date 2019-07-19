#ifndef _DATA_GENERATOR_
#define _DATA_GENERATOR_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataBuffer.hpp"

template<typename Tdata, typename Taccess=unsigned char>
class CDataGenerator: public CDataBuffer<Tdata, Taccess>
{

public:

  CDataGenerator(std::vector<omp_lock_t*> &lock)
  : CDataBuffer<Tdata, Taccess>(lock)
  {
    this->debug=true;
    this->class_name="CDataGenerator";
    this->check_locks(lock);
  }//constructor

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
  {
    if(this->debug)
    {
      this->lprint.print("",false);
      printf("4 B%02d #%04d: ",n,i);fflush(stdout);
      access.print("access",false);fflush(stderr);
      this->lprint.unset_lock();
    }
    //wait lock
    unsigned int c=0;
    this->laccess.wait_for_status(access[n],this->STATUS_FREE,this->STATE_FILLING, c);//free,filling

    //fill image
    images[n].fill(i);

    //set filled
    this->laccess.set_status(access[n],this->STATE_FILLING,this->STATUS_FILLED, this->class_name[5],i,n,c);//filling,filled
  }//iteration

};//CDataGenerator

#endif //_DATA_GENERATOR_

