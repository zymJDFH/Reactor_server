#include "EchoServer.h"
int main(int argc, char const *argv[])
{
     if(argc!=3){
        std::cout<<"ip port"<<std::endl;
        return -1;
    }
    EchoServer echoserver(argv[1],atoi(argv[2]));
    echoserver.Start();
    return 0;
}
