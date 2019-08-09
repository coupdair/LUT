//CoolImage
#include "CImg.h"

//C++ base
#include <iostream>
#include <string>

//OpenMP
#include <omp.h>
#include <vector>

//thread lock
#include "CDataGenerator.hpp"
#include "CDataStore.hpp"

//! \todo [high] class: v baseOMPLock, v +print, v +access _ +progress, _ +buffer, v +run: acc, gen,store

using namespace cimg_library;

#define VERSION "v0.1.7"

#define S 0 //sample

//types
typedef unsigned int Taccess;
typedef unsigned int Tdata;

int main(int argc,char **argv)
{
  ///command arguments, i.e. CLI option
  cimg_usage(std::string("generate and store data.\n" \
  " It uses different GNU libraries (see --info option)\n\n" \
  " usage: ./store -h -I\n" \
  "        ./store -s 1024 -n 123 -X true -o sample.cimg && ls sample_000???.cimg\n" \
  "\n version: "+std::string(VERSION)+"\n compilation date:" \
  ).c_str());//cimg_usage

  const char* imagefilename = cimg_option("-o","sample.cimg","output file name (e.g. \"-o data.cimg -d 3\" gives data_???.cimg)");
  const int digit=cimg_option("-d",6,  "number of digit for file names");
  const int width=cimg_option("-s",1024, "size   of vector");
  const int count=cimg_option("-n",123,  "number of vector");
  const int nbuffer=cimg_option("-b",12, "size   of vector buffer (total size is b*s*4 Bytes)");
  const int threadCount=cimg_option("-c",0,"thread count");
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

  //OpenMP
  if(threadCount>0)
  {//user number of thread
    omp_set_dynamic(0);
    omp_set_num_threads(threadCount);
  }//user

  //OpenMP locks
  omp_lock_t print_lock;omp_init_lock(&print_lock);

  //! circular buffer
  CImgList<Tdata> images(nbuffer,width,1,1,1);
  images[0].fill(0);
  images[0].print("image",false);
  //access locking
  omp_lock_t lck;omp_init_lock(&lck);

  //! access and status of buffer
  std::vector<Taccess> v_access(nbuffer);
  std::vector<Taccess> i_access(nbuffer);
  CImgList<Taccess> access(2);

  //! thread locks
  std::vector<omp_lock_t*> locks;locks.push_back(&print_lock);locks.push_back(&lck);

  //! print access values
  {
  CDataBuffer<Tdata,Taccess> temp(locks);
  temp.init_access(v_access,access[0]);
  temp.init_access(i_access,access[1]);
  access.print("access (free state)",false);fflush(stderr);
  }

  #pragma omp parallel shared(print_lock, v_access,i_access,access,images)
  {
  int id=omp_get_thread_num(),tn=omp_get_num_threads();
  #pragma omp single
  {
  if(tn<2) {printf("error: run error, this process need at least 2 threads (presently only %d available)\n",tn);exit(2);}
  else {printf("info: running %d threads\n",tn);fflush(stdout);}
  }//single

  //run threads
  switch(id)
  {
    case 0:
    {//generate
      CDataGenerator<Tdata,Taccess> generate(locks);
      generate.init_access(v_access,i_access,access);
      generate.run(access,images, count);
      break;
    }//generate
    case 1:
    {//store
      CDataStore<Tdata,Taccess> store(locks,imagefilename,digit);
      store.init_access(v_access,i_access,access);
      store.concurrent_run(access,images, count);
      break;
    }//store
    case 2:
    {//store
      CDataStore<Tdata,Taccess> store(locks,imagefilename,digit);
      store.init_access(v_access,i_access,access);
      store.concurrent_run(access,images, count);
      break;
    }//store
  }//switch(id)
  }//parallel section

  access.print("access (free state)",false);fflush(stderr);
  images.print("CImgList",false);
  return 0;
}//main

