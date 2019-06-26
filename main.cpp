#include "CImg.h"
#include <iostream>
#include <string>

//OpenMP
#include <omp.h>
#include <vector>

//! \todo [high] class: CBaseOMPLock, +print, +progress, +buffer, +run: gen,store

using namespace cimg_library;

#define VERSION "v0.0.5"

#define S 0 //sample

class CBaseOMPLock
{
public:
  std::string class_name;
  int id;
  int tn;

  CBaseOMPLock(std::vector<omp_lock_t*> &lock){class_name="CBaseOMPLock";id=omp_get_thread_num();tn=omp_get_num_threads();}
  virtual void unset_lock(){}
  virtual void print(char* message, bool unset=true){printf("class=%s\n",class_name.c_str());printf(message);}
};//CBaseOMPLock

class CPrintOMPLock: public CBaseOMPLock
{
public:
  omp_lock_t *p_print_lock;
  CPrintOMPLock(std::vector<omp_lock_t*> &lock):CBaseOMPLock(lock){class_name="CPrintOMPLock";if(lock.size()>0) p_print_lock=lock[0];else{printf("code error: locks should have at least 1 lock for %s class.",class_name.c_str());exit(99);}}
  virtual void unset_lock(){omp_unset_lock(p_print_lock);}
  virtual void print(char* message, bool unset=true)
  {//locked section
    omp_set_lock(p_print_lock);
    printf("class=%s\n",class_name.c_str());
    printf("t%d/%d %s",id,tn,message);
    fflush(stdout);
    if(unset) omp_unset_lock(p_print_lock);
  }//print
};//CPrintOMPLock

int main(int argc,char **argv)
{
  ///command arguments, i.e. CLI option
  cimg_usage(std::string("generate and store data.\n" \
  " It uses different GNU libraries (see --info option)\n\n" \
  " usage: ./store -h -I\n" \
  "        ./store -s 1024 -n 123 -X true -o sample.cimg && ls sample_000???.cimg\n" \
  "\n version: "+std::string(VERSION)+"\n compilation date:" \
  ).c_str());//cimg_usage

  const char* imagefilename = cimg_option("-o","sample.cimg","output file name");
  const int width=cimg_option("-s",1024,  "size   of vector");
  const int count=cimg_option("-n",123,   "number of vector");
  const int nbuffer=cimg_option("-b",12,  "size   of vector buffer (total size is b*s*4 Bytes)");
  ///standard options
  #if cimg_display!=0
  const bool show_X=cimg_option("-X",true,NULL);//-X hidden option
  bool show=cimg_option("--show",show_X,"show GUI (or -X option)");show=show_X|show;//same --show or -X option
  #endif
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help; //same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
  bool show_version=cimg_option("-v",false,NULL);//-v hidden option
  if( cimg_option("--version",show_version,"show version (or -v option)") ) {show_version=true;std::cout<<VERSION<<std::endl;return 0;}//same --version or -v option
  if(show_help) {/*print_help(std::cerr);*/return 0;}
  //}CLI option

  //OpenMP locks
  omp_lock_t print_lock;omp_init_lock(&print_lock);
  std::vector<omp_lock_t*> locks;locks.push_back(&print_lock);
  //OpenMP print
  CPrintOMPLock pr(locks);

  //! circular buffer
  CImgList<unsigned int> images(nbuffer,width,1,1,1);
  images[0].fill(0);
  images[0].print("image",false);
  //locking
  omp_lock_t lck;
  omp_init_lock(&lck);
  locks.push_back(&lck);
  //! access and status of buffer
  CImg<unsigned char> access(nbuffer,1,1,1);
  access.fill(0);//free
  access.print("access (free state)",false);fflush(stdout);

  #pragma omp parallel shared(pr,print_lock,locks, access,lck, images)
  {
  int id=omp_get_thread_num(),tn=omp_get_num_threads();
  #pragma omp single
  {
  if(tn<2) {printf("error: run error, this process need at least 2 threads (presently only %d available)\n",tn);exit(2);}
  else {printf("info: running %d threads\n",tn);fflush(stdout);}
  }//single
  for(int n=0,i=0;i<count;++i,++n)
  {
/**/
  if(id<2)//2 for 2 threads
  {//locked section
  pr.print(" message ",false);
  printf("|t%d/%d|",id,tn);//temp
  printf("4 B%02d #%04d: ",n,i);fflush(stdout);
  access.print("access",false);fflush(stderr);
  pr.unset_lock();
  }//lock
/**/
    switch(id)
    {
      case 0:
      {//generate
        unsigned int c=0;
        unsigned char a=99;
        do
        {//waiting for free
        //locked section
        {
          omp_set_lock(&lck);
          a=access[n];
          if(a==0x0)/*free*/ access[n]=0xF;//filling
          omp_unset_lock(&lck);
        }//lock
          ++c;
        }while(a!=0x0);//waiting for free

        //fill image
        images[n].fill(i);

        //locked section
        {
          omp_set_lock(&lck);
          //debug
          if(access[n]!=0xF)/*filling*/ {printf("error: code error, acces should be 0xF i.e. Filling for buffer#%d (presently is is 0x%x)",n,access[n]);omp_unset_lock(&lck);exit(99);}
          access[n]=0x1;//filled

          //debug misc.
//          printf("G%d/%d 4 B%02d #%04d wait=%d\n",id,tn,n,i,c);fflush(stdout);

          omp_unset_lock(&lck);
        }//lock
        break;
      }//generate
      case 1:
      {//store
        unsigned int c=0;
        unsigned char a=99;
        do
        {//waiting for filled
        //locked section
        {
          omp_set_lock(&lck);
          a=access[n];
          if(a==0x1)/*filled*/ access[n]=0x5;//storing
          omp_unset_lock(&lck);
        }//lock
          ++c;
        }while(a!=0x1);//waiting for filled

        //save image
        CImg<char> nfilename(1024);
        cimg::number_filename(imagefilename,i,6,nfilename);
        images[n].save_cimg(nfilename);

        //locked section
        {
          omp_set_lock(&lck);
          //debug
          if(access[n]!=0x5)/*storing*/ {printf("error: code error, acces should be 0x5 i.e. Storing for buffer#%d (presently is is 0x%x)",n,access[n]);omp_unset_lock(&lck);exit(99);}
          access[n]=0x0;//free

          //debug misc.
//          printf("S%d/%d 4 B%02d #%04d wait=%d\n",id,tn,n,i,c);fflush(stdout);

          omp_unset_lock(&lck);
        }//lock

        break;
      }//store
    }//switch(id)
    //circular buffer
    if(n==nbuffer-1) n=-1;
  }//vector loop
  }//parallel section

  access.print("access (free state)",false);fflush(stdout);
  images.print("CImgList",false);
  return 0;
}//main

