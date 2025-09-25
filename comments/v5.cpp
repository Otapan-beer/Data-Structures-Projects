#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <functional> // For std::function

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

    std::shared_ptr<CommentNode> parseCommentLine(const std::string& line) {
        std::function<std::string(const std::string&)> extractField = [&line](const std::string& fieldName) -> std::string {
            size_t pos = line.find("\"" + fieldName + "\": \"");
            if (pos == std::string::npos) {
                pos = line.find("\"" + fieldName + "\": \"");
                if (pos == std::string::npos) {
                    if (fieldName == "parent_comment_id" || fieldName == "crawled_date") {
                        return "";
                    }
                    return "0";
                }
            }
            pos += fieldName.length() + 4;
            size_t end = line.find("\"", pos);
            return line.substr(pos, end - pos);
        };

        std::string video_id = extractField("video_id");
        std::string author = extractField("author");
        std::string comment_id = extractField("comment_id");
        int like_count = std::stoi(extractField("like_count"));
        int reply_count = std::stoi(extractField("reply_count"));
        bool is_reply = extractField("is_reply") == "true";
        std::string parent_comment_id = extractField("parent_comment_id");
        std::string published_date = extractField("published_date");
        std::string crawled_date = extractField("crawled_date");
        bool is_video_owner = extractField("is_video_owner") == "true";
        std::string comment_text = extractField("comment");

        return std::make_shared<CommentNode>(video_id, author, comment_id, like_count, 
                                           reply_count, is_reply, parent_comment_id,
                                           published_date, crawled_date, is_video_owner, 
                                           comment_text);
    }

    void markSubtreeDeleted(const std::string& comment_id) {
        if (comment_map.find(comment_id) == comment_map.end()) return;
        
        deleted_comments.insert(comment_id);
        for (const auto& child : comment_map[comment_id]->children) {
            markSubtreeDeleted(child->comment_id);
        }
    }

    void displayCommentHelper(const std::shared_ptr<CommentNode>& node, 
                            std::ofstream& outFile, 
                            int depth = 0) {
        if (deleted_comments.find(node->comment_id) != deleted_comments.end()) {
            return;
        }
        
        std::string indent(depth * 4, ' ');
        outFile << indent << node->author << " " << node->published_date << "\n";
        outFile << indent << node->comment << "\n";
        outFile << indent;
        if (node->like_count > 0) outFile << "\U0001F44D " << node->like_count;
        else outFile << "0 likes";
        outFile << " | " << node->children.size() << " replies\n\n";

        for (const auto& child : node->children) {
            if (deleted_comments.find(child->comment_id) == deleted_comments.end()) {
                displayCommentHelper(child, outFile, depth + 1);
            }
        }
    }

public:
    void loadCommentsFromJSON(const std::string& filename) {
        std::ifstream jsonFile(filename);
        if (!jsonFile.is_open()) {
            std::cerr << "Failed to open JSON file: " << filename << std::endl;
            exit(1);
        }

        std::string line;
        while (std::getline(jsonFile, line)) {
            std::shared_ptr<CommentNode> comment = parseCommentLine(line);
            if (!comment) continue;

            comment_map[comment->comment_id] = comment;

            if (comment->parent_comment_id.empty()) {
                existing_roots.push_back(comment);
            } else {
                if (comment_map.find(comment->parent_comment_id) != comment_map.end()) {
                    comment_map[comment->parent_comment_id]->children.push_back(comment);
                }
            }
        }
        jsonFile.close();
    }

    void processOperations(const std::string& filename, const std::string& output_file) {
        std::ifstream opsFile(filename);
        if (!opsFile.is_open()) {
            std::cerr << "Failed to open operations file: " << filename << std::endl;
            exit(1);
        }

        std::ofstream clearFile(output_file, std::ios::trunc);
        clearFile.close();

        std::string command;
        while (opsFile >> command) {
            if (command == "reply_to_video") {
                std::string comment_id, author, comment_text;
                opsFile >> comment_id >> author;
                opsFile >> std::ws;
                std::getline(opsFile, comment_text, '"');
                
                if (!comment_text.empty() && comment_text.front() == '"') {
                    comment_text = comment_text.substr(1);
                }
                if (!comment_text.empty() && comment_text.back() == '"') {
                    comment_text.pop_back();
                }

                std::shared_ptr<CommentNode> node = std::make_shared<CommentNode>(
                    "", author, comment_id, 0, 0, false, "",
                    "0 seconds ago", "", false, comment_text
                );
                comment_map[comment_id] = node;
                new_roots.push_back(node);
            }
            else if (command == "reply_to_comment") {
                std::string parent_id, comment_id, author, comment_text;
                opsFile >> parent_id >> comment_id >> author;
                opsFile >> std::ws;
                std::getline(opsFile, comment_text, '"');
                
                if (!comment_text.empty() && comment_text.front() == '"') {
                    comment_text = comment_text.substr(1);
                }
                if (!comment_text.empty() && comment_text.back() == '"') {
                    comment_text.pop_back();
                }

                std::shared_ptr<CommentNode> node = std::make_shared<CommentNode>(
                    "", author, comment_id, 0, 0, true, parent_id,
                    "0 seconds ago", "", false, comment_text
                );
                comment_map[comment_id] = node;

                if (comment_map.find(parent_id) != comment_map.end()) {
                    comment_map[parent_id]->children.push_back(node);
                }
            }
            else if (command == "like_comment") {
                std::string comment_id;
                opsFile >> comment_id;
                if (comment_map.find(comment_id) != comment_map.end()) {
                    comment_map[comment_id]->like_count++;
                }
            }
            else if (command == "delete_comment") {
                std::string comment_id;
                opsFile >> comment_id;
                markSubtreeDeleted(comment_id);
            }
            else if (command == "display_comment") {
                std::string comment_id;
                opsFile >> comment_id;
                
                std::ofstream outFile(output_file, std::ios::app);
                if (comment_map.find(comment_id) != comment_map.end()) {
                    displayCommentHelper(comment_map[comment_id], outFile);
                }
                outFile.close();
            }
        }
        opsFile.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " input1.json input2.txt output.txt\n";
        return 1;
    }

    CommentManager manager;
    manager.loadCommentsFromJSON(argv[1]);
    manager.processOperations(argv[2], argv[3]);
    
    return 0;
}