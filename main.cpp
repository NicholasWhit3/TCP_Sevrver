#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>

int main()
{
//create a socket
    int listening = socket(AF_INET,SOCK_STREAM,0);
    if(listening == -1)
    {
        std::cerr << "Can't creat socket" << std::endl;
        return -1;
    }
//bind the socket to a IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54001);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }
//mark the socket for listening in

    if(listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen" << std::endl;
        return -3;
    }

//Accept a call
    sockaddr_in client;
    socklen_t cleintSize = sizeof(client);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    int clientSocket = accept(listening, (sockaddr*)&client, &cleintSize);

    if(clientSocket == -1)
    {
        std::cerr << "Problem with client connection" << std::endl;
        return -4;
    }

//close listening socket
   close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);

    if(result)
    {
        std::cout << host << " connected on " << service << std::endl;
    }else{
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }


    char buffer[4096];
    while(true)
    {
        memset(buffer, 0, 4096);
        int bytesReceive = recv(clientSocket, buffer, 4096, 0);
        if(bytesReceive == -1)
        {
            std::cerr << "There was connection issue" <<std::endl;
            break;
        }

        if(bytesReceive == 0)
        {
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << std::string(buffer, 0, bytesReceive) << std::endl;

        send(clientSocket, buffer, bytesReceive + 1, 0);

    }

    close(clientSocket);
}
