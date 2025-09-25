#include "comment_manager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " input1.json input2.txt output.txt\n";
        return 1;
    }

    CommentManager manager;
    
    // Load existing comments from JSON
    manager.loadCommentsFromJSON(argv[1]);
    
    // Process operations from input file
    manager.processOperations(argv[2]);
    
    // Write output to file
    manager.writeOutput(argv[3]);
    
    return 0;
}