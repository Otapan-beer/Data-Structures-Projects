#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>

// TreeNode structure to represent a comment
struct TreeNode {
    std::string comment_id;
    std::string author;
    std::string comment;
    int like_count;
    bool is_video_owner;
    std::vector<std::shared_ptr<TreeNode>> children;

    TreeNode(const std::string& id, const std::string& auth, const std::string& comm, int likes, bool owner)
        : comment_id(id), author(auth), comment(comm), like_count(likes), is_video_owner(owner) {}
};

// Class to manage the comment tree
class CommentManager {
private:
    std::unordered_map<std::string, std::shared_ptr<TreeNode>> comment_map;
    std::vector<std::shared_ptr<TreeNode>> root_comments;

    void displayCommentHelper(const std::shared_ptr<TreeNode>& node, std::ofstream& output, int indent = 0) {
        if (!node) return;
        output << std::string(indent, ' ') << node->author << ": " << node->comment << " (" << node->like_count << " likes)";
        if (node->is_video_owner) output << " [Video Owner]";
        output << "\n";
        for (const auto& child : node->children) {
            displayCommentHelper(child, output, indent + 4);
        }
    }

public:
    void loadComments(const std::string& json_file) {
        std::ifstream file(json_file);
        if (!file.is_open()) {
            std::cerr << "Failed to open the JSON file." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::string video_id, author, comment_id, parent_comment_id, comment;
            int like_count;
            bool is_reply, is_video_owner;

            // Parse the JSON line (simplified parsing for this example)
            size_t pos = line.find("\"comment_id\":");
            comment_id = line.substr(pos + 14, line.find("\"", pos + 14) - (pos + 14));

            pos = line.find("\"author\":");
            author = line.substr(pos + 10, line.find("\"", pos + 10) - (pos + 10));

            pos = line.find("\"comment\":");
            comment = line.substr(pos + 11, line.find("\"", pos + 11) - (pos + 11));

            pos = line.find("\"like_count\":");
            like_count = std::stoi(line.substr(pos + 13, line.find(",", pos + 13) - (pos + 13)));

            pos = line.find("\"is_reply\":");
            is_reply = line.substr(pos + 11, 4) == "true";

            pos = line.find("\"parent_comment_id\":");
            parent_comment_id = line.substr(pos + 22, line.find("\"", pos + 22) - (pos + 22));

            pos = line.find("\"is_video_owner\":");
            is_video_owner = line.substr(pos + 18, 4) == "true";

            auto node = std::make_shared<TreeNode>(comment_id, author, comment, like_count, is_video_owner);
            comment_map[comment_id] = node;

            if (is_reply && !parent_comment_id.empty()) {
                if (comment_map.find(parent_comment_id) != comment_map.end()) {
                    comment_map[parent_comment_id]->children.push_back(node);
                }
            } else {
                root_comments.push_back(node);
            }
        }
        file.close();
    }

    void replyToComment(const std::string& parent_id, const std::string& comment_id, const std::string& author, const std::string& comment) {
        auto node = std::make_shared<TreeNode>(comment_id, author, comment, 0, false);
        comment_map[comment_id] = node;

        if (comment_map.find(parent_id) != comment_map.end()) {
            comment_map[parent_id]->children.push_back(node);
        }
    }

    void replyToVideo(const std::string& comment_id, const std::string& author, const std::string& comment) {
        auto node = std::make_shared<TreeNode>(comment_id, author, comment, 0, false);
        comment_map[comment_id] = node;
        root_comments.push_back(node);
    }

    void likeComment(const std::string& comment_id) {
        if (comment_map.find(comment_id) != comment_map.end()) {
            comment_map[comment_id]->like_count++;
        }
    }

    void deleteComment(const std::string& comment_id) {
        if (comment_map.find(comment_id) != comment_map.end()) {
            auto node = comment_map[comment_id];
            for (auto& root : root_comments) {
                if (root == node) {
                    root = nullptr;
                    break;
                }
            }
            comment_map.erase(comment_id);
        }
    }

    void displayComment(const std::string& comment_id, const std::string& output_file) {
        std::ofstream output(output_file, std::ios::app);
        if (!output.is_open()) {
            std::cerr << "Failed to open the output file." << std::endl;
            return;
        }

        if (comment_map.find(comment_id) != comment_map.end()) {
            displayCommentHelper(comment_map[comment_id], output);
        }
        output.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: nycomments.exe input1.json input2.txt output.txt" << std::endl;
        return 1;
    }

    std::string json_file = argv[1];
    std::string input_file = argv[2];
    std::string output_file = argv[3];

    CommentManager manager;
    manager.loadComments(json_file);

    std::ifstream opsFile(input_file);
    if (!opsFile.is_open()) {
        std::cerr << "Failed to open the operations file." << std::endl;
        return 1;
    }

    std::string command;
    while (opsFile >> command) {
        if (command == "reply_to_video") {
            std::string comment_id, author, comment;
            opsFile >> comment_id >> author;
            opsFile >> std::ws;
            std::getline(opsFile, comment, '"');
            manager.replyToVideo(comment_id, author, comment);
        } else if (command == "reply_to_comment") {
            std::string parent_id, comment_id, author, comment;
            opsFile >> parent_id >> comment_id >> author;
            opsFile >> std::ws;
            std::getline(opsFile, comment, '"');
            manager.replyToComment(parent_id, comment_id, author, comment);
        } else if (command == "like_comment") {
            std::string comment_id;
            opsFile >> comment_id;
            manager.likeComment(comment_id);
        } else if (command == "delete_comment") {
            std::string comment_id;
            opsFile >> comment_id;
            manager.deleteComment(comment_id);
        } else if (command == "display_comment") {
            std::string comment_id;
            opsFile >> comment_id;
            manager.displayComment(comment_id, output_file);
        }
    }

    opsFile.close();
    return 0;
}