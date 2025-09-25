#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include <string>
#include <vector>

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
    
    std::vector<CommentNode*> replies;
    
    // Constructor
    CommentNode(const std::string& vid, const std::string& auth, const std::string& cid,
               int likes, int replies, bool is_rep, const std::string& parent_id,
               const std::string& pub_date, const std::string& crawl_date,
               bool is_owner, const std::string& cmt);
    
    // Destructor
    ~CommentNode();
    
    // Add a reply to this comment
    void addReply(CommentNode* reply);
    
    // Delete this comment and all its replies
    void deleteComment();
};

#endif // COMMENT_NODE_H