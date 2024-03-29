#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include <string.h>
#include <fstream>
#include <cstdint>
#import<thread>
#include <poll.h>

#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
using namespace std;


#define no_argument 0
#define required_argument 1
#define optional_argument 2

std::string getURL(std::string imageName);
void * handleClient(void * arg);
int checkTimeout(int new_socket);
int time_out;
pthread_mutex_t threadCountLock;
pthread_mutex_t logLock;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char *argv[]) {

    int opt;



    int socketDesc, new_socket;


    int port = 2012;
    int rate_msgs = 3; //requests
    int rate_time = 60; //Seconds
    int max_users = 3;
    time_out = 80; //Seconds






    //Command Line Args
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const struct option longopts[] = {
            {"PORT",      optional_argument, NULL, 'P'},
            {"RATE_MSGS", optional_argument, NULL, 'R'},
            {"RATE_TIME", optional_argument, NULL, 'T'},
            {"MAX_USERS", optional_argument, NULL, 'M'},
            {"TIME_OUT",  optional_argument, NULL, 'O'},
            {0, 0, 0,                              0}
    };


    while (((opt = getopt_long(argc, argv, "PRTMO", longopts, NULL)) != -1)) {

        //std::cout << "Arguement is optarg: " << optarg << std::endl;


        switch (opt) {
            case 'P':
                port = atoi(optarg);
                break;
            case 'R':
                rate_msgs = atoi(optarg);
                break;
            case 'T':
                rate_time = atoi(optarg);
                break;
            case 'M':
                max_users = atoi(optarg);
                break;
            case 'Q':
                time_out = atoi(optarg);
                break;
            case '?':
                std::cout << "Error: Unknown argument";
                return 1;
        }
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    cout << "Server is running on port " << port << " with the following arguments..";
    std::cout << "\nRate_msgs: " << rate_msgs;
    std::cout << "\nRate_time: " << rate_time;
    std::cout << "\nMax_users: " << max_users;
    std::cout << "\nTime_Out: " << time_out << endl;

    //Opens socket
    if ((socketDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cout << "\nError, unable to create socket";
        return 1;

    }

    //Creates and sets address struct
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //Sets Address to  localhost
    address.sin_port = htons(port);


    //Binds Socket
    if (bind(socketDesc, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cout << "\nUnable to bind";
        return 1;
    }

    //Listens on socket
    if (listen(socketDesc, 3) < 0) {
        std::cout << "\nFailed to listen";
        return 1;
    }

    std::cout << "\nListening...\n";


    //Accepts new connection as new_socket
//        if ((new_socket = accept(socketDesc, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
//            std::cout << "Error accepting";
//        }
//        std::cout << "Accepted client: " << new_socket;

    int client_sock;
    pthread_t * threads = (pthread_t *) malloc(sizeof(pthread_t) * max_users);
    int userCnt = 0;

    while ((client_sock = accept(socketDesc, (struct sockaddr *) &address, (socklen_t *) &addrlen)) != -1) {
        if(userCnt < max_users){
            pthread_create(&threads[userCnt], NULL, handleClient, &client_sock);
            std::cout << "\nCreated thread: " << userCnt << endl;
            userCnt++;
        }
    }



}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void * handleClient(void * arg){

    int new_socket = *(int*)arg;

    while(1) {
        //reads in size of QR Code,
        std::uint32_t buffer;

        std::uint32_t returnCode = 0;



        if(checkTimeout(new_socket) == 1){
            returnCode = 2;
        }

        int BR_QRImageSize = read(new_socket, &buffer, sizeof(std::uint32_t));
        std::cout << "\nRecieved size: " << buffer;
        if (buffer >= 20000) {
            std::cout << "Client sent file too large. ";
            returnCode = 1;
        }


        if(checkTimeout(new_socket) == 1){
            returnCode = 2;
        }

        //Creates buffer of needed size for QR Code Image
        char *recImgBuffer = (char *) malloc(buffer);

        //Reads in and stores QR code image named based on socket FD
        int BR_QRImage = read(new_socket, recImgBuffer, buffer);
        FILE *recievedFile;
        std::string fileName = "imageFromClient" + to_string(new_socket);
        recievedFile = fopen(fileName.c_str(), "w");
        fwrite(recImgBuffer, buffer, 1, recievedFile);
        fclose(recievedFile);



        //Gets URL from QR Code
        std::string URL = getURL(fileName.c_str());



        if (URL == "-1") {
            std::cout << "\ngetURL returned invalid QRCode";
            returnCode = 1;
            URL = "";
            int BS_returnCode = send(new_socket, &returnCode, sizeof(std::uint32_t), 0);


        }

        switch(returnCode){
            case 0:

                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
        }

        //Sends return code
        int BS_returnCode = send(new_socket, &returnCode, sizeof(std::uint32_t), 0);
        std::cout << "\nSent return code: " << returnCode;

        //Gets size of URL and sends it.
        std::uint32_t URLSize = strlen(URL.c_str());
        int BS_URLlength = send(new_socket, &URLSize, sizeof(std::uint32_t), 0);
        std::cout << "\nSent Size of URL: " << strlen(URL.c_str());

        //Sends URL to client
        int BS_URL = send(new_socket, URL.c_str(), strlen(URL.c_str()), 0);
        std::cout << "\nSent URL: " << URL;


        free(recImgBuffer);



    }
    close(new_socket);
    return 0;

}

int checkTimeout(int new_socket){
    struct pollfd pollStruct;
    pollStruct.fd = new_socket;
    pollStruct.events = POLLIN;
    int retval = poll(&pollStruct, 1, time_out * 1000);
    if (retval == 0){
        std::cout << "\nTime Out\n";
        //close(new_socket);
        //std::cout << "\nThread from: " << new_socket << " closing\n";
        //pthread_exit(0);
        return 1;
    }
}



#pragma clang diagnostic pop

std::string getURL(std::string imageName){

    std::string blankCmd = "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner " + imageName;
    char * cmd = (char *) blankCmd.c_str();

    FILE* pipe = popen(cmd, "r");
    char urlBuffer[20000];
    int count = 0;
    while(fgets(urlBuffer, 10000, pipe)){


        if(count == 0){
            std::string bufferString(urlBuffer);
            std::size_t found = bufferString.find("No barcode found");
            if(found != -1){
                std::string notFound = "-1";
                return notFound;
            }
        }

        //std::cout << std::endl << urlBuffer;

        if(count == 4){
            //std::cout << std::endl << urlBuffer;
            break;
        }
        count++;
    }
    std::string URL(urlBuffer);
    return URL;
}
