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

  //! compution kernel for an iteration (compution=copy, here)
  virtual void kernelGPU(compute::vector<char> &in,compute::vector<char> &out)
  {
    //compute with lambda
    using compute::lambda::_1;
    compute::transform(device_vector1.begin(), device_vector1.end(), device_vector3.begin(),
      _1 , queue);
  };//kernelGPU

  //! compution kernel for an iteration
  virtual void kernel(CImg<Tdata> &in,CImg<Tdata> &out)
  {
    //copy CPU to GPU
    compute::copy(in.begin(), in.end(), device_vector1.begin(), queue);
    //compute
    kernelGPU(device_vector1,device_vector3);
    //copy GPU to CPU
    compute::copy(device_vector3.begin(), device_vector3.end(), out.begin(), queue);
  };//kernel

};//CDataProcessorGPU

#endif //_DATA_PROCESSOR_GPU_

