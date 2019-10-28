#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include <string.h>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    int opt;
    int index;

    int port = 0;
    int rate_msgs = 0;
    int rate_time = 0;
    int max_users = 0;
    int time_out = 0;

    const struct option longopts[] = {
            {"PORT", optional_argument, 0, 'P'},
            {"RATE_MSGS", optional_argument, 0, 'R'},
            {"RATE_TIME", optional_argument, 0, 'T'},
            {"MAX_USERS", optional_argument, 0, 'M'},
            {"TIME_OUT", optional_argument, 0, 'O'},
    };

    while(((opt = getopt_long(argc, argv, "PRTMO", longopts, &index)) != -1)){
        switch(opt){
            case 'P':
                port = stoi(optarg);
                break;
            case 'R':
                rate_msgs = stoi(optarg);
                break;
            case 'T':
                rate_time = stoi(optarg);
                break;
            case 'M':
                max_users = stoi(optarg);
                break;
            case 'Q':
                time_out = stoi(optarg);
                break;
        }
    }

    std::cout << "Entered: Port: " << port;



    return 0;
}