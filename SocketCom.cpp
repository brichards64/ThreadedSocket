#include "SocketCom.h"

SocketCom::SocketCom(bool client, int port, char *buffer, bool *newbuff){
 
  m_buffer= buffer;
  m_newbuff=newbuff;
 
  sockfd=socket(AF_INET, SOCK_STREAM, 0);//create socket (adress domain, socket type, protacall 0=auto) 
  if (sockfd<0) std::cout<<"Failed to open socket"<<std::endl; 

  if(client){
    server = gethostbyname("localhost");
    if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
    }  
  }

  bzero((char *) &serv_addr, sizeof(serv_addr)); // has to be zeroed
  portno = port;
  serv_addr.sin_family = AF_INET; //interet based adressing
  if(!client) serv_addr.sin_addr.s_addr = INADDR_ANY; //own comps ip}
  serv_addr.sin_port = htons(portno); //prt /horns converts to network byte order
  
  if(!client){
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)   std::cout<<"Failed to bind socket"<<std::endl;
  }
}

void SocketCom::ListenStart(){
  for (int i=0; i<5;i++){
    tcontrol[i]=-1;
  }
  pthread_create (&threads[0], NULL, SocketCom::Listen, this); 
  
}

void SocketCom::ListenStop(){
  
  pthread_mutex_lock (&mu_tcontrol);
  for (int i=0; i<5;i++){
    tcontrol[i]=0;
  }
  pthread_mutex_unlock (&mu_tcontrol);
  
  
}



void *SocketCom::Listen(void* arg){
  SocketCom* self=(SocketCom*)arg;

  pthread_mutex_lock (&self->mu_tcontrol);
  while(self->tcontrol[0]){
    pthread_mutex_unlock (&self->mu_tcontrol);
    
    for(int tnum=1;tnum<5;tnum++){
      
      pthread_mutex_lock (&self->mu_tcontrol);      
      if (self->tcontrol[tnum]==-1){
	
	pthread_mutex_unlock (&self->mu_tcontrol);       
	
	listen(self->sockfd,5);
	self->clilen[tnum] = sizeof(self->cli_addr[tnum]);
	self->newsockfd[tnum] = accept(self->sockfd,(struct sockaddr *) &self->cli_addr[tnum],&self->clilen[tnum]);
	if (self->newsockfd[tnum] < 0)   {
	  std::cout<<"Failed to accept"<<std::endl;
	}    
	else {
	  pthread_create (&self->threads[tnum], NULL, SocketCom::ListenThread, &tnum);
	}
      }
      pthread_mutex_unlock (&self->mu_tcontrol); 
    }
    pthread_mutex_unlock (&self->mu_tcontrol);
  }
  pthread_mutex_unlock (&self->mu_tcontrol);
  std::cout<<"thread 0 exiting"<<std::endl;
  pthread_exit(NULL);
  
}

void *SocketCom::ListenThread(void* arg){
  int tnum=(int)arg;
  
  pthread_mutex_lock (&mu_tcontrol);
  while(tcontrol[tnum]){
    pthread_mutex_unlock (&mu_tcontrol);
    
    pthread_mutex_lock (&mu_newbuff);
    while(!*m_newbuff){
      
      bzero(m_buffer,256);
      n = read(newsockfd[tnum],m_buffer,255);
      if (n < 0)  std::cout<<"Failed to read from socket"<<std::endl;
      //printf("Here is the message: %s\n",m_buffer);
      
      
      n = write(newsockfd[tnum],"I got your message",18);
      if (n < 0)  std::cout<<"Failed to write to socket"<<std::endl;
      
      *m_newbuff=true;
      pthread_mutex_unlock (&mu_newbuff);
    }
    pthread_mutex_unlock (&mu_newbuff);
    
  }
  tcontrol[tnum]=-1;
  pthread_mutex_unlock (&mu_tcontrol);
  std::cout<<"thread "<<tnum<<" exiting"<<std::endl;
  pthread_exit(NULL);
  
}

void SocketCom::Send(){
 bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  n=connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

    if(n < 0)  std::cout<<"Failed to connect"<<std::endl;
    else{
    std::cout<<"n= "<<n<<std::endl;
  bzero(m_buffer,256);
  fgets(m_buffer,255,stdin);
  n = write(sockfd,m_buffer,strlen(m_buffer));
  if (n < 0) std::cout<<"Failed writing to socket"<<std::endl;
  bzero(m_buffer,256);
  n = read(sockfd,m_buffer,255);
  if (n < 0) std::cout<<"Failed to read from socket"<<std::endl;
    printf("%s\n",m_buffer);
    }
}
