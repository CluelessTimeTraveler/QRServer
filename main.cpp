#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    int opt;
    int index;
    char buffer[1024] = {0};
    char *hello = "Hello from server yeet";


    int socketDesc, new_socket;

    //extern char *optarg;

    int port = 2012;
    int rate_msgs = 3; //requests
    int rate_time = 60; //Seconds
    int max_users = 3;
    int time_out = 80; //Seconds


    struct sockaddr_in address;
    int addrlen = sizeof(address);

//    const struct option longopts[] = {
//            {"PORT", optional_argument, NULL, 'P'},
//            {"RATE_MSGS", optional_argument, NULL, 'R'},
//            {"RATE_TIME", optional_argument, NULL, 'T'},
//            {"MAX_USERS", optional_argument, NULL, 'M'},
//            {"TIME_OUT", optional_argument, NULL, 'O'},
//            {0, 0, 0, 0}
//    };
//
//    std::cout << "Yeet2\n";
//
//    while(((opt = getopt_long(argc, argv, "PRTMO", longopts, NULL)) != -1)){
//
//        std::cout << "Arguement is optarg: " << optarg << std::endl;
//
//
//        switch(opt){
//            case 'P':
//                port = atoi(optarg);
//                break;
//            case 'R':
//                rate_msgs = atoi(optarg);
//                break;
//            case 'T':
//                rate_time = atoi(optarg);
//                break;
//            case 'M':
//                max_users = atoi(optarg);
//                break;
//            case 'Q':
//                time_out = atoi(optarg);
//                break;
//        }
//    }

    std::cout << "Entered: \nPort: " << port;
    std::cout << "\nRate_msgs: " << rate_msgs;
    std::cout << "\nRate_time: " << rate_time;
    std::cout << "\nMax_users: " << max_users;
    std::cout << "\nTime_Out: " << time_out;

    if((socketDesc = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP)) < 0){
        std::cout << "\nError, unable to create socket";

    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(socketDesc, (struct sockaddr *)&address, sizeof(address)) < 0){
        std::cout << "\nUnable to bind";
    }

    if(listen(socketDesc, 3) < 0){
        std::cout << "\nFailed to listen";
    }

    if((new_socket = accept(socketDesc, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
        std::cout << "Error accepting";
    }

    int value = read(new_socket, buffer, 1024);

    printf("%s\n",buffer);
    send(new_socket, hello,strlen(hello),0);
    printf("Hello Message sent\n");

    return 0;
}