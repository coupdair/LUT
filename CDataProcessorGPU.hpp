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

//! \todo 3 flowing lines to remove (inheritance or other thread !)
  CImg<unsigned char> host_vector3;
  std::string file_name;
  unsigned int file_name_digit;

  // create vectors on the device
  compute::vector<char> device_vector1;
  compute::vector<char> device_vector2;
  compute::vector<char> device_vector3;

  CDataProcessorGPU(std::vector<omp_lock_t*> &lock
  , compute::device device, int VECTOR_SIZE
  , std::string imagefilename, unsigned int digit
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  )
  : CDataProcessor<Tdata, Taccess>(lock,wait_status,set_status,wait_statusR,set_statusR)
  , ctx(device), queue(ctx, device)
  , host_vector3(VECTOR_SIZE)
  , device_vector1(VECTOR_SIZE, ctx), device_vector2(VECTOR_SIZE, ctx), device_vector3(VECTOR_SIZE, ctx)
  {
    this->debug=true;
    this->class_name="CDataProcessorGPU";
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
    this->laccess.wait_for_status(access[n],this->STATUS_RECEIVED,this->STATE_PROCESSING, c);//received, processing

    //copy CPU to GPU
    unsigned int n1,n2;
    if(n>0)
      n1=n-1;
    else
      n1=images.size()-1;
    n2=n;
    compute::copy(images[n1].begin(), images[n1].end(), device_vector1.begin(), queue);
    compute::copy(images[n2].begin(), images[n2].end(), device_vector2.begin(), queue);

    //compute
    using compute::lambda::_1;
    using compute::lambda::_2;
    compute::transform(device_vector1.begin(), device_vector1.end(), device_vector2.begin(), device_vector3.begin(),
      _1+_2 , queue);

    //copy GPU to CPU
    compute::copy(
      device_vector3.begin(), device_vector3.end(), host_vector3.begin(), queue
    );

    /*
    //compution check
    int err = 0;
    for(unsigned int j=0; j<host_vector3.size(); ++j)
    {
      if(host_vector3[j]!=image[j]+image2[j])
      {
        ++err;
      }
    }
    std::cout << "Errors : " << err << "/" << host_vector3.size() << std::endl;
    */

    //store
    CImg<char> nfilename(1024);
    cimg::number_filename(file_name.c_str(),i,file_name_digit,nfilename);
    host_vector3.save_png(nfilename);

    this->laccess.set_status(access[n],this->STATE_PROCESSING,this->STATUS_PROCESSED, this->class_name[5],i,n,c);//processing, processed -> storage

  }//iteration

};//CDataProcessorGPU

#endif //_DATA_PROCESSOR_GPU_

