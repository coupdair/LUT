#ifndef _DATA_PROCESSOR_MORPHO_
#define _DATA_PROCESSOR_MORPHO_


//Package CImg
#include <CImg.h>

using namespace cimg_library;

#include "CDataProcessor.hpp"

template<typename Tdata,typename Tdout, typename Taccess=unsigned char>
class CDataProcessor_dilate : public CDataProcessor<Tdata,Tdout, Taccess>
{
public:
  //! width of the structuring element
  Tdata s;
  CDataProcessor_dilate(std::vector<omp_lock_t*> &lock
  , Tdata s
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_status=CDataAccess::STATUS_FILLED
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_status=CDataAccess::STATUS_PROCESSED
  , CDataAccess::ACCESS_STATUS_OR_STATE wait_statusR=CDataAccess::STATUS_FREE
  , CDataAccess::ACCESS_STATUS_OR_STATE  set_statusR=CDataAccess::STATUS_FILLED
  )
  : CDataProcessor<Tdata,Tdout, Taccess>(lock,wait_status,set_status,wait_statusR,set_statusR)
  , s(s)
  {
//    this->debug=true;
    this->class_name="CDataProcDilate";
  }//constructor

  //! compution kernel for an iteration
  virtual void kernel(CImg<Tdata> &in,CImg<Tdout> &out)
  {
    out=in.get_dilate(s);
  };//kernel

};//CDataProcessor_dilate

#endif //_DATA_PROCESSOR_MORPHO_

