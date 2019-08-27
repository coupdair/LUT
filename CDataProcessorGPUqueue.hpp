#ifndef _DATA_PROCESSOR_GPU_QUEUE_
#define _DATA_PROCESSOR_GPU_QUEUE_


//Packages Boost
#include <vector>
#include <iostream>
#include <boost/compute.hpp>

//Package CImg
#include <CImg.h>

namespace compute = boost::compute;

using namespace cimg_library;

using compute::lambda::_1;

#include "CDataProcessorGPU.hpp"

//! base queueing for GPU process (ment for enqueue and dequeue)
/**
 * This class might be used for debug or test only.
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessorGPUqueue : public CDataProcessorGPU<Tdata, Taccess>
{
public:
/*  CImgList<Tdata> *image_p;
  std::vector<compute::command_queue> *queue_p;
  // create vectors on the device
  std::vector<compute::vector<Tdata> > *device_vector1_p;
  std::vector<compute::vector<Tdata> > *device_vector3_p;
*/
  CDataProcessorGPU(std::vector<omp_lock_t*> &lock
  , compute::device device, int vector_size
//  , CImgList<Tdata> *image_p, std::vector<compute::command_queue> *queue_p
//  , std::vector<compute::vector<Tdata> > *device_vector1_p
//  , std::vector<compute::vector<Tdata> > *device_vector3_p
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  )
  : CDataProcessorGPU<Tdata, Taccess>(lock,device,vector_size,wait_status,set_status,wait_statusR,set_statusR)
//  , image_p(image_p), queue_p(queue_p)
//  , device_vector1_p(device_vector1_p), device_vector3_p(device_vector3_p)
  {
    this->debug=true;
    this->class_name="CDataProcessorGPUqueue";
/*    if((*image_p)(0).width()!=vector_size) {std::cout<< __FILE__<<"/"<<__func__;printf("(...) code error: bad image size"); exit(99);}
    if( (*image_p).size()!=(*queue_p).size()
    ||  (*image_p).size()!=(*device_vector1_p).size()
    ||  (*image_p).size()!=(*device_vector3_p).size()
    ) {std::cout<< __FILE__<<"/"<<__func__;printf("(...) code error: different buffer sizes"); exit(99);}
*/
    this->check_locks(lock);
  }//constructor

  //! compution kernel for an iteration (compution=copy, here)
  virtual void kernelGPU(compute::vector<Tdata> &in,compute::vector<Tdata> &out
  , compute::command_queue &queue
  , compute::vector<Tdata> &device_vector1, compute::vector<Tdata> &device_vector3)
  {
    //compute with lambda
    using compute::lambda::_1;
    compute::transform(device_vector1.begin(), device_vector1.end(), device_vector3.begin(),
      _1 , queue);
  };//kernelGPU

  //! compution kernel for an enqueue iteration
  virtual void kernel(CImg<Tdata> &in,CImg<Tdata> &out
  , compute::command_queue &queue
  , compute::vector<Tdata> &device_vector1, compute::vector<Tdata> &device_vector3)
  {
    //copy CPU to GPU
    compute::copy(in.begin(), in.end(), device_vector1.begin(), queue);
    //compute
    kernelGPU(device_vector1,device_vector3,queue);
    //copy GPU to CPU
    compute::copy(device_vector3.begin(), device_vector3.end(), out.begin(), queue);
  };//kernel

  //! one iteration for any enqueue loop
  virtual void iteration_enqueue(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
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
    this->laccess.wait_for_status(access[n],this->wait_status,this->STATE_ENQUEUEING, c);//filled, processing
    //compution in local
//    kernel(images[n],image_p[n] ,(*queue_p)[n],(*device_vector1_p)[n],(*device_vector3_p)[n]);
    kernel(images[n],this->image ,this->queue,this->device_vector1,this->device_vector3);
    //unlock
    this->laccess.set_status(access[n],this->STATE_ENQUEUEING,/*this->set_status*/this->STATUS_QUEUED, this->class_name[5],i,n,c);//processing, processed

  }//iteration_enqueue

  //! one iteration for any dequeue loop
  virtual void iteration_dequeue(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
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
    this->laccess.wait_for_status(access[n],/*this->wait_status*/this->STATUS_QUEUED,this->STATE_PROCESSING, c);//filled, processing
    //compution in local
//    (*queue_p)[n].finish();
    queue.finish();
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
//    results[n]=image_p(n);
    results[n]=this->image;
    //unlock
    this->laccessR.set_status(accessR[n],this->STATE_PROCESSING,this->set_statusR, this->class_name[5],i,n,c);//processing, processed

  }//iteration_dequeue

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    this->iteration_enqueue(access,images, accessR,results, n,i);
    this->iteration_dequeue(access,images, accessR,results, n,i);
  }//iteration

};//CDataProcessorGPUqueue

/* \todo [highest] en/dequeue class implementation
//CDataProcessorGPUenqueue
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    this->iteration_enqueue(access,images, accessR,results, n,i);
    this->iteration_dequeue(access,images, accessR,results, n,i);
  }//iteration

//CDataProcessorGPUdequeue
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    this->iteration_enqueue(access,images, accessR,results, n,i);
    this->iteration_dequeue(access,images, accessR,results, n,i);
  }//iteration
*/

#endif //_DATA_PROCESSOR_GPU_QUEUE_

