#include "comment_manager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

CommentManager::CommentManager() {}

CommentManager::~CommentManager() {
    for (CommentNode* comment : topLevelComments) {
        delete comment;
    }
    for (CommentNode* comment : newTopLevelComments) {
        delete comment;
    }
}

void CommentManager::loadCommentsFromJSON(const std::string& filename) {
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        std::cerr << "Failed to open JSON file: " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(jsonFile, line)) {
        CommentNode* comment = parseCommentLine(line);
        if (comment) {
            if (comment->parent_comment_id.empty()) {
                topLevelComments.push_back(comment);
            } else {
                auto parentIt = commentMap.find(comment->parent_comment_id);
                if (parentIt != commentMap.end()) {
                    parentIt->second->addReply(comment);
                }
            }
            commentMap[comment->comment_id] = comment;
        }
    }
    jsonFile.close();
}

CommentNode* CommentManager::parseCommentLine(const std::string& line) {
    // Implementation to parse JSON line and create CommentNode
    // (Would extract each field from the JSON string)
    return nullptr; // Placeholder - actual implementation needed
}

void CommentManager::processOperations(const std::string& filename) {
    std::ifstream opsFile(filename);
    if (!opsFile.is_open()) {
        std::cerr << "Failed to open operations file: " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(opsFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        if (tokens[0] == "reply_to_video") {
            processReplyToVideo(tokens);
        } else if (tokens[0] == "reply_to_comment") {
            processReplyToComment(tokens);
        } else if (tokens[0] == "like_comment") {
            processLikeComment(tokens);
        } else if (tokens[0] == "delete_comment") {
            processDeleteComment(tokens);
        } else if (tokens[0] == "display_comment") {
            processDisplayComment(tokens);
        }
    }
    opsFile.close();
}

// Implementations of processReplyToVideo, processReplyToComment, etc.
// would go here...

void CommentManager::displayComment(CommentNode* node, std::ofstream& outFile, int depth) {
    std::string indent(depth * 4, ' ');
    
    outFile << indent << "Author: " << node->author << std::endl;
    outFile << indent << "Published: " << node->published_date << std::endl;
    outFile << indent << "Likes: " << node->like_count;
    if (node->like_count > 0) {
        outFile << " \U0001F44D";
    }
    outFile << std::endl;
    outFile << indent << "Reply count: " << node->reply_count << std::endl;
    outFile << indent << "Comment: " << node->comment << std::endl;
    outFile << std::endl;
    
    for (CommentNode* reply : node->replies) {
        displayComment(reply, outFile, depth + 1);
    }
}

void CommentManager::writeOutput(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        exit(1);
    }

    // Display existing top-level comments first
    for (CommentNode* comment : topLevelComments) {
        displayComment(comment, outFile);
    }

    // Then display new top-level comments
    for (CommentNode* comment : newTopLevelComments) {
        displayComment(comment, outFile);
    }

    outFile.close();
}