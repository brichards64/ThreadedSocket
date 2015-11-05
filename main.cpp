#include <iostream>

#include "SocketCom.h"

int main(){
  
  char *buffer;
  bool *newbuff;
  
  buffer=new char[256];
  newbuff=new bool;
  *newbuff=false;
  //std::cout<<"wtf"<<std::endl;
  //std::cout<<"buff or "<<newbuff<<std::endl;
  //std::cout<<"*butt or "<<&newbuff<<std::endl;
  
  SocketCom server(false,"heppc076",24001,buffer,newbuff);
  //  while (true){
  //std::cout<<"ben debug 1"<<std::endl;
  server.ListenStart();
  //std::cout<<"ben debug 2"<<std::endl;
  while (true){
    if(*newbuff){
      //std::cout<<"ben debug 3"<<std::endl;
      std::cout<<buffer<<std::endl;
      *newbuff=false;
    }
    //   usleep(10000000);
    //  server.ListenStop();
    //  
  }
  
  return 0;
  
}
