#include <iostream>
#include <string>
#include "utilv4.h"

//we are left with a tiny little main file
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: nytrends.exe input.json output.txt hashtag|sound" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string mode = argv[3];

    if (mode != "hashtag" && mode != "sound") { //wrong input
        std::cerr << "Invalid mode. Use 'hashtag' or 'sound'" << std::endl;
        return 1;
    }

    processFile(inputFile, outputFile, mode); //call the function in h file
    return 0; //ended successfully
}