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
  CImg<unsigned char> image;
  CAccessOMPLock laccessR;
  CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR;
  CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR;

  CDataProcessor(std::vector<omp_lock_t*> &lock
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE waitStatusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  setStatusR=CDataAccess::STATUS_FILLED
  )
  : CDataBuffer<Tdata, Taccess>(lock,wait_status,set_status)
//  , wait_statusR(wait_statusR), set_statusR(set_statusR)
  {
    this->debug=true;
    this->class_name="CDataProcessor";
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 locks for %s class.",this->class_name.c_str());
      exit(99);
    }
    this->check_locks(lock);

    this->wait_statusR=waitStatusR;
    this->set_statusR= setStatusR;
  }//constructor

  //! one iteration for any loop
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
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
    this->laccess.wait_for_status(access[n],this->wait_status,this->STATE_PROCESSING, c);//filled, processing

    //compution
    unsigned int n1,n2;
    if(n>0)
      n1=n-1;
    else
      n1=images.size()-1;
    n2=n;
    image=images[n1]+images[n2];

    this->laccess.set_status(access[n],this->STATE_PROCESSING,this->set_status, this->class_name[5],i,n,c);//processing, processed

    //wait lock
    this->laccessR.wait_for_status(accessR[n],this->wait_statusR,this->STATE_PROCESSING, c);//filled, processing

    //copy
    results[n]=image;

    //set filled
    this->laccessR.set_status(accessR[n],this->STATE_PROCESSING,this->set_statusR, this->class_name[5],i,n,c);//processing, processed

  }//iteration

};//CDataProcessor

#endif //_DATA_PROCESSOR_

