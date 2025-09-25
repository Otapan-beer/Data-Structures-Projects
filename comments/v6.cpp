#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <functional>

class CommentNode {
public:
    std::string video_id;
    std::string author;
    std::string comment_id;
    int like_count;
    int reply_count;
    bool is_reply;
    std::string parent_comment_id;
    std::string published_date;
    std::string crawled_date;
    bool is_video_owner;
    std::string comment;
    
    std::vector<std::shared_ptr<CommentNode>> children;
    bool is_new = false;
    
    CommentNode(const std::string& vid, const std::string& auth, const std::string& cid,
               int likes, int replies, bool is_rep, const std::string& parent_id,
               const std::string& pub_date, const std::string& crawl_date,
               bool is_owner, const std::string& cmt)
        : video_id(vid), author(auth), comment_id(cid), like_count(likes), reply_count(replies),
          is_reply(is_rep), parent_comment_id(parent_id), published_date(pub_date),
          crawled_date(crawl_date), is_video_owner(is_owner), comment(cmt) {}
};

class CommentManager {
private:
    std::unordered_map<std::string, std::shared_ptr<CommentNode>> comment_map;
    std::vector<std::shared_ptr<CommentNode>> existing_roots;
    std::vector<std::shared_ptr<CommentNode>> new_roots;
    std::unordered_set<std::string> deleted_comments;
    std::vector<std::string> comments_to_display;

    void logError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }

    void logInfo(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }

    std::shared_ptr<CommentNode> parseCommentLine(const std::string& line) {
        try {
            if (line.empty() || line[0] != '{' || line.back() != '}') {
                logError("Invalid JSON line format");
                return nullptr;
            }

            auto extractField = [&line](const std::string& fieldName) -> std::string {
                size_t fieldPos = line.find("\"" + fieldName + "\":");
                if (fieldPos == std::string::npos) return "";

                size_t valueStart = line.find('"', fieldPos + fieldName.length() + 3);
                if (valueStart == std::string::npos) return "";

                size_t valueEnd = line.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) return "";

                return line.substr(valueStart + 1, valueEnd - valueStart - 1);
            };

            std::string comment_id = extractField("comment_id");
            if (comment_id.empty()) {
                logError("Comment ID is empty in line: " + line);
                return nullptr;
            }

            std::string video_id = extractField("video_id");
            std::string author = extractField("author");
            std::string comment_text = extractField("comment");
            std::string parent_comment_id = extractField("parent_comment_id");
            std::string published_date = extractField("published_date");
            std::string crawled_date = extractField("crawled_date");
            
            int like_count = 0;
            try {
                like_count = std::stoi(extractField("like_count"));
            } catch (...) {
                logError("Invalid like_count for comment: " + comment_id);
            }

            int reply_count = 0;
            try {
                reply_count = std::stoi(extractField("reply_count"));
            } catch (...) {
                logError("Invalid reply_count for comment: " + comment_id);
            }

            bool is_reply = extractField("is_reply") == "true";
            bool is_video_owner = extractField("is_video_owner") == "true";

            return std::make_shared<CommentNode>(
                video_id, author, comment_id, 
                like_count, reply_count, is_reply, 
                parent_comment_id, published_date, crawled_date, 
                is_video_owner, comment_text
            );
        } catch (const std::exception& e) {
            logError("JSON parsing error: " + std::string(e.what()));
            return nullptr;
        }
    }

    void displayCommentHelper(const std::shared_ptr<CommentNode>& node, 
                            std::ofstream& outFile, 
                            int depth = 0) {
        if (!node) {
            logError("Attempted to display null comment node");
            return;
        }

        if (deleted_comments.find(node->comment_id) != deleted_comments.end()) {
            logInfo("Skipping deleted comment: " + node->comment_id);
            return;
        }
        
        try {
            std::string indent(depth * 4, ' ');
            outFile << indent << node->author << " " << node->published_date << "\n";
            outFile << indent << node->comment << "\n";
            outFile << indent;
            
            if (node->like_count > 0) {
                outFile << "\U0001F44D " << node->like_count;
            } else {
                outFile << "0 likes";
            }
            
            outFile << "\n\n";

            for (const auto& child : node->children) {
                displayCommentHelper(child, outFile, depth + 1);
            }
        } catch (const std::exception& e) {
            logError("Error displaying comment " + node->comment_id + ": " + e.what());
        }
    }

public:
    void loadCommentsFromJSON(const std::string& filename) {
        logInfo("Loading comments from: " + filename);
        std::ifstream jsonFile(filename);
        if (!jsonFile.is_open()) {
            logError("Failed to open JSON file: " + filename);
            exit(1);
        }

        std::string line;
        int line_num = 0;
        while (std::getline(jsonFile, line)) {
            line_num++;
            try {
                if (line.empty()) continue;
                
                std::shared_ptr<CommentNode> comment = parseCommentLine(line);
                if (!comment) {
                    logError("Failed to parse comment at line " + std::to_string(line_num));
                    continue;
                }

                comment_map[comment->comment_id] = comment;

                if (comment->parent_comment_id.empty()) {
                    existing_roots.push_back(comment);
                    logInfo("Added root comment: " + comment->comment_id);
                } else {
                    if (comment_map.find(comment->parent_comment_id) != comment_map.end()) {
                        comment_map[comment->parent_comment_id]->children.push_back(comment);
                        logInfo("Added reply " + comment->comment_id + " to parent " + comment->parent_comment_id);
                    } else {
                        logError("Parent comment not found for: " + comment->comment_id);
                    }
                }
            } catch (const std::exception& e) {
                logError("Error processing line " + std::to_string(line_num) + ": " + e.what());
            }
        }
        jsonFile.close();
        logInfo("Finished loading " + std::to_string(comment_map.size()) + " comments");
    }

    void processOperations(const std::string& filename, const std::string& output_file) {
        logInfo("Processing operations from: " + filename);
        std::ifstream opsFile(filename);
        if (!opsFile.is_open()) {
            logError("Failed to open operations file: " + filename);
            exit(1);
        }

        std::string line;
        int line_num = 0;
        while (std::getline(opsFile, line)) {
            line_num++;
            try {
                if (line.empty()) continue;
                
                std::istringstream iss(line);
                std::string command;
                iss >> command;

                if (command == "reply_to_video") {
                    std::string comment_id, author, comment_text;
                    iss >> comment_id >> author;
                    iss >> std::ws;
                    std::getline(iss, comment_text, '"');
                    
                    if (!comment_text.empty() && comment_text.front() == '"') {
                        comment_text = comment_text.substr(1);
                    }
                    if (!comment_text.empty() && comment_text.back() == '"') {
                        comment_text.pop_back();
                    }

                    auto node = std::make_shared<CommentNode>(
                        "", author, comment_id, 0, 0, false, "",
                        "0 seconds ago", "", false, comment_text
                    );
                    node->is_new = true;
                    comment_map[comment_id] = node;
                    new_roots.push_back(node);
                    logInfo("Added new video reply: " + comment_id);
                }
                else if (command == "reply_to_comment") {
                    std::string parent_id, comment_id, author, comment_text;
                    iss >> parent_id >> comment_id >> author;
                    iss >> std::ws;
                    std::getline(iss, comment_text, '"');
                    
                    if (!comment_text.empty() && comment_text.front() == '"') {
                        comment_text = comment_text.substr(1);
                    }
                    if (!comment_text.empty() && comment_text.back() == '"') {
                        comment_text.pop_back();
                    }

                    auto node = std::make_shared<CommentNode>(
                        "", author, comment_id, 0, 0, true, parent_id,
                        "0 seconds ago", "", false, comment_text
                    );
                    node->is_new = true;
                    comment_map[comment_id] = node;

                    if (comment_map.find(parent_id) != comment_map.end()) {
                        comment_map[parent_id]->children.push_back(node);
                        logInfo("Added new reply " + comment_id + " to parent " + parent_id);
                    } else {
                        logError("Parent comment not found for new reply: " + comment_id);
                    }
                }
                else if (command == "like_comment") {
                    std::string comment_id;
                    iss >> comment_id;
                    if (comment_map.find(comment_id) != comment_map.end()) {
                        comment_map[comment_id]->like_count++;
                        logInfo("Liked comment: " + comment_id);
                    } else {
                        logError("Comment not found for like: " + comment_id);
                    }
                }
                else if (command == "delete_comment") {
                    std::string comment_id;
                    iss >> comment_id;
                    deleted_comments.insert(comment_id);
                    logInfo("Marked comment for deletion: " + comment_id);
                }
                else if (command == "display_comment") {
                    std::string comment_id;
                    iss >> comment_id;
                    comments_to_display.push_back(comment_id);
                    logInfo("Queued comment for display: " + comment_id);
                }
                else {
                    logError("Unknown command at line " + std::to_string(line_num) + ": " + command);
                }
            } catch (const std::exception& e) {
                logError("Error processing command at line " + std::to_string(line_num) + ": " + e.what());
            }
        }
        opsFile.close();

        logInfo("Writing output to: " + output_file);
        std::ofstream outFile(output_file, std::ios::trunc);
        if (!outFile.is_open()) {
            logError("Failed to open output file: " + output_file);
            exit(1);
        }

        for (const auto& comment_id : comments_to_display) {
            if (comment_map.find(comment_id) != comment_map.end()) {
                if (deleted_comments.find(comment_id) == deleted_comments.end()) {
                    logInfo("Displaying comment: " + comment_id);
                    displayCommentHelper(comment_map[comment_id], outFile);
                } else {
                    logInfo("Skipping deleted comment for display: " + comment_id);
                }
            } else {
                logError("Comment not found for display: " + comment_id);
            }
        }
        outFile.close();
        logInfo("Finished processing operations");
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " input1.json input2.txt output.txt\n";
        return 1;
    }

    CommentManager manager;
    try {
        manager.loadCommentsFromJSON(argv[1]);
        manager.processOperations(argv[2], argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}