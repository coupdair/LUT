#ifndef _DATA_BUFFER_
#define _DATA_BUFFER_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataAccess.hpp"

//! base class intended to circular buffer sub-classes
/**
 * this base class declares \c constructor, \c iteration and \c run functions for circular buffer with shared access.
 * It is implementing \c run functions, i.e. loop on circular buffer, so \c iteration should be defined in sub-classes.
 * Data is shared, so both circular access and lock to it should be provided (see parameters: \c images, \c access, \c lock).
**/
template<typename Tdata, typename Taccess=unsigned char>
class CDataBuffer: public CDataAccess
{
public:
  //access buffers
  //! access buffer as vector
  std::vector<Taccess> *vector_access;
  //! index  buffer as vector
  std::vector<Taccess> *vector_index;

  CDataBuffer(std::vector<omp_lock_t*> &lock)
  : CDataAccess(lock)
  {
    debug=true;
    class_name="CDataBuffer";
    check_locks(lock);
    vector_access=NULL;
    vector_index=NULL;
  }//constructor

  //! inititialise cimg data on vector data
  virtual void init_on_vector(std::vector<Taccess> &vec,CImg<Taccess> &img,std::vector<Taccess> *p_vec)
  {
    //shared data
    img.assign(vec.data(),vec.size(),1,1,1,true);
    //vector reference
    p_vec=&vec;
  };//init_cimg_on_vector

  //! setup access array
  virtual void init_access(std::vector<Taccess> &v_access,CImg<Taccess> &i_access)
  {
    //values
    std::fill(v_access.begin(),v_access.end(),STATUS_FREE);
    //shared data
    init_on_vector(v_access,i_access,vector_access);
  };//init_access

  //! setup access array
  virtual void init_access(std::vector<Taccess> &v_access,std::vector<Taccess> &v_index, CImgList<Taccess> &access)
  {
    //values
    std::fill(v_access.begin(),v_access.end(),STATUS_FREE);
    std::fill(v_index.begin(), v_index.end(), STATUS_FREE);
    //shared data
    init_on_vector(v_access,access[0], vector_access);
    init_on_vector(v_index, access[1],  vector_index);
  };//init_access

  //! one iteration for any loop
  virtual void iteration(CImgList<Taccess> &access,CImgList<Tdata> &images, int n)
  {
    std::cout<< __FILE__<<"/"<<__func__<<"(buffer="<<access.width()<<", buffer="<<images.size()<<", buffer="<<n<<") empty, should be implemented in inherited class."<<std::endl<<std::flush;
  };//iteration

  //! run for loop
  virtual void run(CImgList<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    int nbuffer=images.size();
    for(int n=0,i=0;i<count;++i,++n)
    {
      this->iteration(access,images, n);
      //circular buffer
      if(n==nbuffer-1) n=-1;
    }//vector loop
  }//run
/*
  //! run for loop
  virtual void concurrent_run(CImg<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    std::cout<< __FILE__<<"/"<<__func__<<"(buffer=["<<access.width()<<","<<access.spectrum()<<"], buffer="<<images.size()<<", count="<<count")."<<std::endl<<std::flush;
    if(access.spectrum()<2)
    {
      printf("error: code error, this access container should have at least 2 variables, i.e. [status,index].\n",tn);
      exit(99);
    }//error
    int nbuffer=images.size();
//! \bug need test to stop an infinite loop (no more for)
    for(int i=0;i<count;++i)
    {
      this->concurrent_iteration(access,images, n,i);
    }//vector loop
  }//concurrent_run
*/

};//CDataBuffer

#endif //_DATA_BUFFER_

