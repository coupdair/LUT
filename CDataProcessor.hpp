#ifndef _DATA_PROCESSOR_
#define _DATA_PROCESSOR_


//Package CImg
#include <CImg.h>

using namespace cimg_library;

#include "CDataBuffer.hpp"

template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessor : public CDataBuffer<Tdata, Taccess>
{
public:
  CImg<unsigned char> host_vector3;
  std::string file_name;
  unsigned int file_name_digit;

  CDataProcessor(std::vector<omp_lock_t*> &lock, std::string imagefilename, unsigned int digit) : CDataBuffer<Tdata, Taccess>(lock)
  {
    this->debug=true;
    this->class_name="CDataProcessor";
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 locks for %s class.",this->class_name.c_str());
      exit(99);
    }
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

#ifdef __PROCESS_FILLED__
    this->laccess.wait_for_status(access[n],this->STATUS_FILLED,this->STATE_PROCESSING, c);//filled, processing
#else
    this->laccess.wait_for_status(access[n],this->STATUS_RECEIVED,this->STATE_PROCESSING, c);//received, processing
#endif

    //compution
    unsigned int n1,n2;
    if(n>0)
      n1=n-1;
    else
      n1=images.size()-1;
    n2=n;
    host_vector3=images[n1]+images[n2];

    //store
    CImg<char> nfilename(1024);
    cimg::number_filename(file_name.c_str(),i,file_name_digit,nfilename);
    host_vector3.save_png(nfilename);

#ifdef __FREE__
    this->laccess.set_status(access[n],this->STATE_PROCESSING,this->STATUS_FREE, this->class_name[5],i,n,c);//processing, free
#else
    this->laccess.set_status(access[n],this->STATE_PROCESSING,this->STATUS_PROCESSED, this->class_name[5],i,n,c);//processing, processed
#endif

  }//iteration

};//CDataProcessor

#endif //_DATA_PROCESSOR_

