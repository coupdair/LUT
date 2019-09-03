#ifndef _DATA_PROCESSOR_GPU_QUEUE_
#define _DATA_PROCESSOR_GPU_QUEUE_


//Packages Boost
#include <vector>
#include <iostream>
#include <boost/compute.hpp>
#include <boost/compute/async/future.hpp>

//Package CImg
#include <CImg.h>

namespace compute = boost::compute;

using namespace cimg_library;

using compute::lambda::_1;

#include "CDataProcessorGPU.hpp"

//#define SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
#warning "SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS active (this must be CODE TEST only)"
#endif

//! base queueing for GPU process (ment for enqueue and dequeue)
/**
 * This class might be used for debug or test only.
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessorGPUqueue : public CDataProcessorGPU<Tdata, Taccess>
{
public:
  CImgList<Tdata> images;
//!\todo [high] . setup circular GPU buffer, i.e. with vector for queue_p, device_vector?_p (WiP: compilation or run errors for assign, so try vector<queue*> and vector<vector*>)
  compute::future<void>lwait;
  std::vector<compute::future<void> >waits;
  // create vectors on the device
  std::vector<compute::vector<Tdata>* >device_vector1s;
  std::vector<compute::vector<Tdata>* >device_vector3s;

  CDataProcessorGPUqueue(std::vector<omp_lock_t*> &lock
  , compute::device device, int vector_size
  , CImgList<Tdata> &images, std::vector<compute::future<void> >&waits
  , std::vector<compute::vector<Tdata>* >&device_vector1s
  , std::vector<compute::vector<Tdata>* >&device_vector3s
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  , bool do_check=false
  )
  : CDataProcessorGPU<Tdata, Taccess>(lock,device,vector_size,wait_status,set_status,wait_statusR,set_statusR,do_check)
  , images(images), waits(waits)
  , device_vector1s(device_vector1s), device_vector3s(device_vector3s)
  {
    this->debug=true;
    this->class_name="CDataProcessorGPUqueue";
    //check data size
    if(images(0).width()!=vector_size) {std::cout<< __FILE__<<"/"<<__func__;printf("(...) code error: bad image size"); exit(99);}
    //! single allocation for en/dequeueing, so no 2 allocation times !
    if(device_vector3s.size()==0)
    {
std::cout<< __FILE__<<"/"<<__func__<<"(...) information: allocating device vectors"<<std::endl;
      for(int i=0;i<images.size();++i) device_vector1s.push_back(new compute::vector<Tdata>(vector_size,this->ctx));
      for(int i=0;i<images.size();++i) device_vector3s.push_back(new compute::vector<Tdata>(vector_size,this->ctx));
    }//allocation
    //check buffer size
    if( images.size()!=waits.size()
    ||  images.size()!=device_vector1s.size()
    ||  images.size()!=device_vector3s.size()
    ) {std::cout<< __FILE__<<"/"<<__func__;printf("(...) code error: different buffer sizes"); exit(99);}
images.print("CDataProcessorGPUqueue");
std::cout<< __FILE__<<"/"<<__func__<<"queue size="<<waits.size()<<std::endl;
    this->check_locks(lock);
  }//constructor

  //! compution kernel for an iteration (compution=copy, here)
  virtual void kernelGPU(compute::vector<Tdata> &in,compute::vector<Tdata> &out
  , compute::command_queue &queue)
  {
    //compute with lambda
    using compute::lambda::_1;
    compute::transform(in.begin(), in.end(), out.begin(),
      _1 , queue);
  };//kernelGPU

#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
  //! compution kernel for an enqueue iteration (using queue)
  virtual void kernel(CImg<Tdata> &in,CImg<Tdata> &out
//  , compute::command_queue &queue
  , compute::future<void> &fwait
  , compute::vector<Tdata> &device_vector1, compute::vector<Tdata> &device_vector3)
  {
    //copy CPU to GPU
    compute::copy(in.begin(), in.end(), device_vector1.begin(), this->queue);
    //compute
    kernelGPU(device_vector1,device_vector3,this->queue);
    //copy GPU to CPU
//        compute::copy      (device_vector3.begin(), device_vector3.end(), out.begin(), this->queue);
    fwait=compute::copy_async(device_vector3.begin(), device_vector3.end(), out.begin(), this->queue);
  };//kernel
#else
  //! compution kernel for an enqueue iteration (using future that waits for last copy, i.e. device to host)
  virtual void kernel(CImg<Tdata> &in,CImg<Tdata> &out
  , compute::future<void> &fwait
  , compute::vector<Tdata> &device_vector1, compute::vector<Tdata> &device_vector3)
  {
    //copy CPU to GPU
std::cout<< __FILE__<<"/"<<__func__<<" 1. copy"<<std::endl<<std::flush;
    compute::copy(in.begin(), in.end(), device_vector1.begin(), this->queue);
    //compute
std::cout<< __FILE__<<"/"<<__func__<<" 2. compute"<<std::endl<<std::flush;
    kernelGPU(device_vector1,device_vector3,this->queue);
    //copy GPU to CPU
std::cout<< __FILE__<<"/"<<__func__<<" 3. copy async"<<std::endl<<std::flush;
    fwait=compute::copy_async(device_vector3.begin(), device_vector3.end(), out.begin(), this->queue);
  };//kernel
#endif //!SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS

  //! one iteration for any enqueue loop
  virtual void iteration_enqueue(CImg<Taccess> &access,CImgList<Tdata> &bimages, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
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
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
//    kernel(bimages[n],this->image ,this->queue,this->device_vector1,this->device_vector3);
//    kernel(bimages[n],this->image ,this->lwait,this->device_vector1,this->device_vector3);
//    kernel(bimages[n],this->image ,waits[n],this->device_vector1,this->device_vector3);
    kernel(bimages[n],images[n] ,waits[n],this->device_vector1,this->device_vector3);
#else
//    kernel(bimages[n],images[n] , waits[n],*(device_vector1s[n]),*(device_vector3s[n]));
    kernel(bimages[n],images[n] ,waits[n],this->device_vector1,this->device_vector3);
#endif
        //check
        if(this->do_check)
        {
          if(bimages[n]==i) NULL; else {++(this->check_error);std::cout<<"compution error: bad generate class for this test."<<std::endl<<std::flush;}
        }

    //unlock
    this->laccess.set_status(access[n],this->STATE_ENQUEUEING,/*this->set_status*/this->STATUS_QUEUED, this->class_name[5],i,n,c);//processing, processed

  }//iteration_enqueue

  //! one iteration for any dequeue loop
  virtual void iteration_dequeue(CImg<Taccess> &access,CImgList<Tdata> &bimages, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
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
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
//    this->queue.finish();
//    this->lwait.wait();
    waits[n].wait();
#else
    waits[n].wait();
#endif
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

        //check
        if(this->do_check)
        {
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
//          if(this->image==i) NULL;
          if(images[n]==i) NULL;
#else
          if(images[n]==i) NULL;
#endif
          else
          {
            ++(this->check_error);
            std::cout<<"compution error: bad check (i.e. test failed) on iteration #"<<i<<" (value="<<
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
//            this->image(0)
            images[n](0)
#else
            images[n](0)
#endif
            <<")."<<std::endl<<std::flush;
          }
        }

    //wait lock
    c=0;
    this->laccessR.wait_for_status(accessR[n],this->wait_statusR,this->STATE_PROCESSING, c);//filled, processing
    //copy local to buffer
#ifdef SEQUENTIAL_USE_SINGLE_LOCAL_CONTAINERS
//    results[n]=this->image;
    results[n]=images[n];
#else
    results[n]=images[n];
#endif
    //unlock
    this->laccessR.set_status(accessR[n],this->STATE_PROCESSING,this->set_statusR, this->class_name[5],i,n,c);//processing, processed

  }//iteration_dequeue

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &bimages, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    std::cout<< __FILE__<<"/"<<__func__<<" 1. enqueue"<<std::endl<<std::flush;
    this->iteration_enqueue(access,bimages, accessR,results, n,i);
    std::cout<< __FILE__<<"/"<<__func__<<" 2. dequeue"<<std::endl<<std::flush;
    this->iteration_dequeue(access,bimages, accessR,results, n,i);
    std::cout<< __FILE__<<"/"<<__func__<<" v. done"<<std::endl<<std::flush;
  }//iteration

};//CDataProcessorGPUqueue

//! enqueueing for GPU process
template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessorGPUenqueue : public CDataProcessorGPUqueue<Tdata, Taccess>
{
public:
  CDataProcessorGPUenqueue(std::vector<omp_lock_t*> &lock
  , compute::device device, int vector_size
  , CImgList<Tdata> &images, std::vector<compute::future<void> > &waits
  , std::vector<compute::vector<Tdata> *> &device_vector1s
  , std::vector<compute::vector<Tdata> *> &device_vector3s
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  , bool do_check=false
  )
  : CDataProcessorGPUqueue<Tdata, Taccess>(lock,device,vector_size
    ,images,waits,device_vector1s,device_vector3s
    ,wait_status,set_status,wait_statusR,set_statusR
    ,do_check
    )
  {
    this->debug=true;
    this->class_name="CDataProcessorGPUenqueue";
    this->check_locks(lock);
  }//constructor

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &bimages, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    this->iteration_enqueue(access,bimages, accessR,results, n,i);
  }//iteration
};//CDataProcessorGPUenqueue

//! dequeueing for GPU process
template<typename Tdata, typename Taccess=unsigned char>
class CDataProcessorGPUdequeue : public CDataProcessorGPUqueue<Tdata, Taccess>
{
public:
  CDataProcessorGPUdequeue(std::vector<omp_lock_t*> &lock
  , compute::device device, int vector_size
  , CImgList<Tdata> &images, std::vector<compute::future<void> > &waits
  , std::vector<compute::vector<Tdata> *> &device_vector1s
  , std::vector<compute::vector<Tdata> *> &device_vector3s
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  , bool do_check=false
  )
  : CDataProcessorGPUqueue<Tdata, Taccess>(lock,device,vector_size
    ,images,waits,device_vector1s,device_vector3s
    ,wait_status,set_status,wait_statusR,set_statusR
    ,do_check
    )
  {
    this->debug=true;
    this->class_name="CDataProcessorGPUdequeue";
    this->check_locks(lock);
  }//constructor

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &bimages, CImg<Taccess> &accessR,CImgList<Tdata> &results, int n, int i)
  {
    this->iteration_dequeue(access,bimages, accessR,results, n,i);
  }//iteration
};//CDataProcessorGPUdequeue

#endif //_DATA_PROCESSOR_GPU_QUEUE_

