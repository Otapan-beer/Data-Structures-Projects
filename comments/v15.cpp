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
    std::unordered_set<std::string> displayed_comments;
    std::unordered_map<std::string, int> insertion_index;


    std::string extractField(const std::string& line, const std::string& fieldName) {
        size_t pos = line.find("\"" + fieldName + "\":");
        if (pos == std::string::npos) return "";
        size_t start = line.find('"', pos + fieldName.size() + 3);
        size_t end = line.find('"', start + 1);
        return (start != std::string::npos && end != std::string::npos) ? line.substr(start + 1, end - start - 1) : "";
    }

    std::string extractNumberField(const std::string& line, const std::string& fieldName) {
        size_t pos = line.find("\"" + fieldName + "\":");
        if (pos == std::string::npos) return "0";
        size_t start = line.find(':', pos) + 1;
        size_t end = line.find_first_of(",}\n", start);
        std::string val = line.substr(start, end - start);
        val.erase(std::remove_if(val.begin(), val.end(), ::isspace), val.end());
        return val;
    }

    std::shared_ptr<CommentNode> parseCommentLine(const std::string& line) {
        std::string cid = extractField(line, "comment_id");
        if (cid.empty()) {
            return nullptr;
        }
        try {
            int likes = std::stoi(extractNumberField(line, "like_count"));
            int replies = std::stoi(extractNumberField(line, "reply_count"));
            bool is_reply = extractField(line, "is_reply") == "true";
            bool is_owner = extractField(line, "is_video_owner") == "true";
            std::shared_ptr<CommentNode> node(new CommentNode(
                extractField(line, "video_id"), extractField(line, "author"), cid,
                likes, replies, is_reply, extractField(line, "parent_comment_id"),
                extractField(line, "published_date"), extractField(line, "crawled_date"),
                is_owner, extractField(line, "comment")
            ));
            return node;
        } catch (const std::exception&) {
            return nullptr;
        }
    }

    int countTotalReplies(const std::shared_ptr<CommentNode>& node) {
        int count = 0;
        for (std::vector<std::shared_ptr<CommentNode>>::const_iterator it = node->children.begin(); it != node->children.end(); ++it) {
            if (!deleted_comments.count((*it)->comment_id)) {
                count += 1 + countTotalReplies(*it);
            }
        }
        return count;
    }

    void displayCommentHelper(const std::shared_ptr<CommentNode>& node, std::ofstream& out, int depth = 0, bool is_root = false) {
        if (!node || deleted_comments.count(node->comment_id)) return;
        std::string indent(depth * 4, ' ');
        out << indent << node->author << " " << node->published_date << "\n";
        out << indent << node->comment << "\n";
        out << indent << "\U0001F44D " << node->like_count << "\n";

        if (is_root) {
            int total = countTotalReplies(node);
            if (total > 0) {
                out << indent << total << (total == 1 ? " reply\n" : " replies\n");
            }
        } else {
            int direct = 0;
            for (std::vector<std::shared_ptr<CommentNode>>::const_iterator it = node->children.begin(); it != node->children.end(); ++it) {
                if (!deleted_comments.count((*it)->comment_id)) {
                    ++direct;
                }
            }
            if (direct > 0) {
                out << indent << direct << (direct == 1 ? " reply\n" : " replies\n");
            }
        }

        for (std::vector<std::shared_ptr<CommentNode>>::iterator it = node->children.begin(); it != node->children.end(); ++it) {
            displayCommentHelper(*it, out, depth + 1, false);
        }
    }

public:
    bool loadCommentsFromJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        std::string line;
        int index = 0;
        while (getline(file, line)) {
            std::shared_ptr<CommentNode> node = parseCommentLine(line);
            if (!node) continue;
            comment_map[node->comment_id] = node;
            insertion_index[node->comment_id] = index++;
        }
        for (std::unordered_map<std::string, std::shared_ptr<CommentNode>>::iterator it = comment_map.begin(); it != comment_map.end(); ++it) {
            std::vector<std::shared_ptr<CommentNode>>& kids = it->second->children;
            std::sort(kids.begin(), kids.end(),
                [this](const std::shared_ptr<CommentNode>& a, const std::shared_ptr<CommentNode>& b) {
                    return insertion_index[a->comment_id] < insertion_index[b->comment_id];
                });
        }
        return true;
    }

    void processOperations(const std::string& filename, const std::string& output_file) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
        std::string line;
        bool output_written = false;
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;
            if (cmd == "reply_to_comment") {
                std::string parent_id, cid, author, rest;
                iss >> parent_id >> cid >> author;
                getline(iss, rest);
                rest = rest.substr(rest.find('"') + 1);
                rest = rest.substr(0, rest.rfind('"'));
                std::shared_ptr<CommentNode> node(new CommentNode("", author, cid, 0, 0, true, parent_id, "0 seconds ago", "", false, rest));
                node->is_new = true;
                comment_map[cid] = node;
                if (comment_map.count(parent_id)) {
                    comment_map[parent_id]->children.push_back(node);
                }
            } else if (cmd == "like_comment") {
                std::string cid;
                iss >> cid;
                if (comment_map.count(cid)) {
                    comment_map[cid]->like_count++;
                }
            } else if (cmd == "delete_comment") {
                std::string cid;
                iss >> cid;
                deleted_comments.insert(cid);
            } else if (cmd == "display_comment") {
                std::string cid;
                iss >> cid;
                if (comment_map.count(cid) && !deleted_comments.count(cid)) {
                    std::ofstream out(output_file, std::ios::app);
                    if (!out.is_open()) return;
                    displayCommentHelper(comment_map[cid], out, 0, true);
                    out.close();
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " input1.json input2.txt output.txt\n";
        return 1;
    }

    std::string input_json = argv[1];
    std::string input_ops = argv[2];
    std::string output_file = argv[3];

    CommentManager cm;
    cm.loadCommentsFromJSON(input_json);
    cm.processOperations(input_ops, output_file);

    return 0;
}
