#ifndef COMMENT_MANAGER_H
#define COMMENT_MANAGER_H

#include "comment_node.h"
#include <unordered_map>
#include <vector>
#include <fstream>

class CommentManager {
public:
    CommentManager();
    ~CommentManager();
    
    // Load comments from JSON file
    void loadCommentsFromJSON(const std::string& filename);
    
    // Process operations from input file
    void processOperations(const std::string& filename);
    
    // Write output to file
    void writeOutput(const std::string& filename);
    
private:
    std::vector<CommentNode*> topLevelComments;
    std::vector<CommentNode*> newTopLevelComments;
    std::unordered_map<std::string, CommentNode*> commentMap;
    
    // Helper functions
    CommentNode* parseCommentLine(const std::string& line);
    void displayComment(CommentNode* node, std::ofstream& outFile, int depth = 0);
    void processReplyToVideo(const std::vector<std::string>& tokens);
    void processReplyToComment(const std::vector<std::string>& tokens);
    void processLikeComment(const std::vector<std::string>& tokens);
    void processDeleteComment(const std::vector<std::string>& tokens);
    void processDisplayComment(const std::vector<std::string>& tokens);
};

#endif // COMMENT_MANAGER_H