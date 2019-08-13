#ifndef _DATA_PROCESSOR_
#define _DATA_PROCESSOR_


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

#include "CDataAccess.hpp"

class CDataProcessor : CDataAccess
{
public:
  compute::context ctx;
  compute::command_queue queue;
  CImg<unsigned char> host_vector3;
  std::string file_name;
  unsigned int file_name_digit;
  // create vectors on the device
  compute::vector<char> device_vector1;
  compute::vector<char> device_vector2;
  compute::vector<char> device_vector3;

  CDataProcessor(std::vector<omp_lock_t*> &lock, compute::device device, int VECTOR_SIZE, std::string imagefilename, unsigned int digit/*????????????)*/) : CDataAccess(lock), ctx(device), queue(ctx, device), host_vector3(VECTOR_SIZE), device_vector1(VECTOR_SIZE, ctx), device_vector2(VECTOR_SIZE, ctx), device_vector3(VECTOR_SIZE, ctx)/*????????*/
  {
    debug=true;
    class_name="CDataProcessor";
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 locks for %s class.",class_name.c_str());
      exit(99);
    }

    file_name=imagefilename;
    file_name_digit=digit;
    check_locks(lock);


  }//constructor

  virtual void iteration(CImg<unsigned char> &access, int n, int i, CImg<unsigned char> image, CImg<unsigned char> image2)
  {
    if(debug)
    {
      lprint.print("",false);
      printf("4 B%02d #%04d: ",n,i);fflush(stdout);
      access.print("access",false);fflush(stderr);
      lprint.unset_lock();
    }

    //wait lock
    unsigned int c=0;
    laccess.wait_for_status(access[n],STATUS_RECEIVED,STATE_PROCESSING, c);//received, processing

    compute::copy(
      image.begin(), image.end(), device_vector1.begin(), queue
    );

    compute::copy(
      image2.begin(), image2.end(), device_vector2.begin(), queue
    );

    using compute::lambda::_1;
    using compute::lambda::_2;

    compute::transform(device_vector1.begin(), device_vector1.end(), device_vector2.begin(), device_vector3.begin(),
      _1+_2 , queue);

    compute::copy(
      device_vector3.begin(), device_vector3.end(), host_vector3.begin(), queue
    );

    int err = 0;
    /*for(unsigned int j=0; j<host_vector3.size(); ++j)
    {
      if(host_vector3[j]!=image[j]+image2[j])
      {
        ++err;
      }
    }*/
    //std::cout << "Errors : " << err << "/" << host_vector3.size() << std::endl;

    CImg<char> nfilename(1024);
    cimg::number_filename(file_name.c_str(),i,file_name_digit,nfilename);
    host_vector3.save_png(nfilename);

    laccess.set_status(access[n],STATE_PROCESSING,STATUS_PROCESSED, class_name[5],i,n,c);//processing, processed -> storage

  }
};

#endif
