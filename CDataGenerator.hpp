#ifndef _DATA_GENERATOR_
#define _DATA_GENERATOR_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataBuffer.hpp"

//! generate data into a shared circular buffer
/**
 * this generation data class implements \c iteration function on the data.
 * Data is shared, so both circular access and lock to it should be provided (see parameters: \c images, \c access, \c lock).
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataGenerator: public CDataBuffer<Tdata, Taccess>
{

public:
  //! current index
  int ci;

  CDataGenerator(std::vector<omp_lock_t*> &lock)
  : CDataBuffer<Tdata, Taccess>(lock)
  {
    this->debug=true;
    this->class_name="CDataGenerator";
    this->check_locks(lock);
    ci=0;
  }//constructor

  //! one iteration for any loop
  virtual void iteration(CImgList<Taccess> &access,CImgList<Tdata> &images, int n)
  {
    if(this->debug)
    {
      this->lprint.print("",false);
      printf("4 B%02d #%04d: ",n,ci);fflush(stdout);
      access.print("access",false);fflush(stderr);
      this->lprint.unset_lock();
    }
    //wait lock
    unsigned int c=0;
    this->laccess.wait_for_status(access(0,n),this->STATUS_FREE,this->STATE_FILLING, c);//free,filling
    //index
    access(1,n)=ci;

    //fill image
    images[n].fill(ci);

    //set filled
    this->laccess.set_status(access(0,n),this->STATE_FILLING,this->STATUS_FILLED, this->class_name[5],access(1,n),n,c);//filling,filled

    //next iteration
    ++ci;

  }//iteration

};//CDataGenerator

#endif //_DATA_GENERATOR_

