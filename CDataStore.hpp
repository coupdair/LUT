#ifndef _DATA_STORE_
#define _DATA_STORE_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataBuffer.hpp"

//! store data from a shared circular buffer to files
/**
 * this storage data class implements \c iteration function on the data.
 * Data is shared, so both circular access and lock to it should be provided (see parameters: \c images, \c access, \c lock).
 * \todo [low] \c wait_for_status might be a locking process to ensure fastest unlocking for these storage classes.
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataStore: public CDataBuffer<Tdata, Taccess>
{
public:
  std::string file_name;
  int file_name_digit;

  CDataStore(std::vector<omp_lock_t*> &lock,std::string imagefilename, int digit)
  : CDataBuffer<Tdata, Taccess>(lock)
  {
    this->debug=true;
    this->class_name="CDataStore";
    file_name=imagefilename;
    file_name_digit=digit;
    this->check_locks(lock);
  }//constructor

  //! one iteration for any loop
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
    this->laccess.wait_for_status(access[n],this->STATUS_FILLED,this->STATE_STORING, c);//filled,storing

    //save image
    CImg<char> nfilename(1024);
    cimg::number_filename(file_name.c_str(),i,file_name_digit,nfilename);
    images[n].save_cimg(nfilename);

    //set filled
    this->laccess.set_status(access[n],this->STATE_STORING,this->STATUS_FREE, this->class_name[5],i,n,c);//storing,free
  }//iteration

};//CDataStore


#endif //_DATA_STORE_

