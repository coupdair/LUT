#ifndef _DATA_STORE_
#define _DATA_STORE_

//CoolImage
#include "CImg.h"
using namespace cimg_library;

//thread lock
#include "CDataAccess.hpp"

template<typename Tdata, typename Taccess=unsigned char>
class CDataStore: public CDataAccess
{
public:
  std::string file_name;
  int file_name_digit;

  CDataStore(std::vector<omp_lock_t*> &lock,std::string imagefilename, int digit)
  : CDataAccess(lock)
  {
    debug=true;
    class_name="CDataStore";
    file_name=imagefilename;
    file_name_digit=digit;
    check_locks(lock);
  }//constructor
  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
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
    laccess.wait_for_status(access[n],STATUS_FILLED,STATE_STORING, c);//filled,storing

    //save image
    CImg<char> nfilename(1024);
    cimg::number_filename(file_name.c_str(),i,file_name_digit,nfilename);
    images[n].save_cimg(nfilename);

    //set filled
    laccess.set_status(access[n],STATE_STORING,STATUS_FREE, class_name[5],i,n,c);//storing,free
  }//iteration
};//CDataStore


#endif //_DATA_STORE_

