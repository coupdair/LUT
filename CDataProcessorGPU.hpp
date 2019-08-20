#ifndef _DATA_PROCESSOR_GPU_
#define _DATA_PROCESSOR_GPU_


//Packages Boost
#include <vector>
#include <iostream>
#include <boost/compute.hpp>

//Package CImg
#include <CImg.h>

namespace compute = boost::compute;

using namespace cimg_library;

using compute::lambda::_1;
using compute::lambda::_2;

#include "CDataProcessor.hpp"

template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessorGPU : public CDataProcessor<Tdata, Taccess>
{
public:
  compute::context ctx;
  compute::command_queue queue;

  // create vectors on the device
  compute::vector<char> device_vector1;
  compute::vector<char> device_vector2;
  compute::vector<char> device_vector3;

  CDataProcessorGPU(std::vector<omp_lock_t*> &lock
  , compute::device device, int VECTOR_SIZE
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  )
  : CDataProcessor<Tdata, Taccess>(lock,wait_status,set_status,wait_statusR,set_statusR)
  , ctx(device), queue(ctx, device)
  , device_vector1(VECTOR_SIZE, ctx), device_vector2(VECTOR_SIZE, ctx), device_vector3(VECTOR_SIZE, ctx)
  {
    this->debug=true;
    this->class_name="CDataProcessorGPU";
    this->image.assign(VECTOR_SIZE);
    this->check_locks(lock);
  }//constructor

  //! one iteration for any loop: copy kernel (as average of same image)
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    if(this->debug)
    {
      this->lprint.print("",false);
      printf("4 B%02d #%04d: ",n,i);fflush(stdout);
      access.print("access",false);fflush(stderr);
      this->lprint.unset_lock();
    }
    //! 1. compute from buffer
    //wait lock
    unsigned int c=0;
    this->laccess.wait_for_status(access[n],this->wait_status,this->STATE_PROCESSING, c);//filled, processing

    //copy CPU to GPU
    compute::copy(images[n].begin(), images[n].end(), device_vector1.begin(), queue);
    compute::copy(images[n].begin(), images[n].end(), device_vector2.begin(), queue);

    //compute
    using compute::lambda::_1;
    using compute::lambda::_2;
    compute::transform(device_vector1.begin(), device_vector1.end(), device_vector2.begin(), device_vector3.begin(),
      _1+_2 , queue);

    //copy GPU to CPU
    compute::copy(
      device_vector3.begin(), device_vector3.end(), this->image.begin(), queue
    );

    //unlock
    this->laccess.set_status(access[n],this->STATE_PROCESSING,this->set_status, this->class_name[5],i,n,c);//processing, processed

    //! 2. copy to buffer
    if(this->debug)
    {
      this->lprint.print("",false);
      printf("4 C%02d #%04d: ",n,i);fflush(stdout);
      accessR.print("accessR",false);fflush(stderr);
      this->lprint.unset_lock();
    }
    //wait lock
    c=0;
    this->laccessR.wait_for_status(accessR[n],this->wait_statusR,this->STATE_PROCESSING, c);//filled, processing
    //copy local to buffer
    results[n]=this->image;
    //unlock
    this->laccessR.set_status(accessR[n],this->STATE_PROCESSING,this->set_statusR, this->class_name[5],i,n,c);//processing, processed

  }//iteration

};//CDataProcessorGPU

#endif //_DATA_PROCESSOR_GPU_

