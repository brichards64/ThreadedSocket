#include <iostream>

#include "SocketCom.h"

int main(){

  bool *newbuff;
  char* buffer;
  buffer =new char[256];
  newbuff=new bool;

  //  SocketCom client(true,24000,buffer,newbuff);

  buffer="hello";
  //while(true){
    SocketCom client(true,24001,buffer,newbuff);
    std::cout<<"sending"<<std::endl;
    client.Send();
    //}

  return 0;

}
