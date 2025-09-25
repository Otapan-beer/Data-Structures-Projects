#include "comment_node.h"
#include <algorithm>

CommentNode::CommentNode(const std::string& vid, const std::string& auth, const std::string& cid,
                         int likes, int replies, bool is_rep, const std::string& parent_id,
                         const std::string& pub_date, const std::string& crawl_date,
                         bool is_owner, const std::string& cmt) :
    video_id(vid), author(auth), comment_id(cid), like_count(likes), reply_count(replies),
    is_reply(is_rep), parent_comment_id(parent_id), published_date(pub_date),
    crawled_date(crawl_date), is_video_owner(is_owner), comment(cmt) {}

CommentNode::~CommentNode() {
    for (CommentNode* reply : replies) {
        delete reply;
    }
}

void CommentNode::addReply(CommentNode* reply) {
    replies.push_back(reply);
    reply_count++;
}

void CommentNode::deleteComment() {
    for (CommentNode* reply : replies) {
        reply->deleteComment();
        delete reply;
    }
    replies.clear();
    reply_count = 0;
}