#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

using namespace std;

// --- Notification Base Class ---
class Notification {
public:
    virtual string format() const = 0;
    virtual ~Notification() {}
};

// --- Derived Notification Types ---
class LikeNotification : public Notification {
    string from;
public:
    LikeNotification(const string& f) : from(f) {}
    string format() const override { return from + " liked your post."; }
};

class TagNotification : public Notification {
    string from;
public:
    TagNotification(const string& f) : from(f) {}
    string format() const override { return from + " tagged you in a post."; }
};

class CommentNotification : public Notification {
    string from;
public:
    CommentNotification(const string& f) : from(f) {}
    string format() const override { return from + " commented on your post."; }
};

class FollowNotification : public Notification {
    string from;
public:
    FollowNotification(const string& f) : from(f) {}
    string format() const override { return from + " started following you."; }
};

class MessageRequestNotification : public Notification {
    string from;
public:
    MessageRequestNotification(const string& f) : from(f) {}
    string format() const override { return from + " wants to send you a message."; }
};

// --- User Preferences ---
struct UserPreferences {
    bool pauseAll, likes, tags, comments, follows, messages;
};

unordered_map<string, string> postToOwner;
unordered_map<string, UserPreferences> userPrefs;
unordered_map<string, string> userIdToName;

bool shouldNotify(const string& username, const string& type) {
    if (userPrefs.find(username) == userPrefs.end()) return false;
    UserPreferences prefs = userPrefs[username];
    if (prefs.pauseAll) return false;
    if (type == "likes") return prefs.likes;
    if (type == "tags") return prefs.tags;
    if (type == "comments") return prefs.comments;
    if (type == "follows") return prefs.follows;
    if (type == "messages") return prefs.messages;
    return false;
}

// --- Load JSON Files (Manual Parsing) ---
void loadUsers(const string& path) {
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        size_t idPos = line.find("\"id\": \"") + 7;
        size_t idEnd = line.find("\"", idPos);
        string id = line.substr(idPos, idEnd - idPos);

        size_t usernamePos = line.find("\"username\": \"", idEnd) + 13;
        size_t usernameEnd = line.find("\"", usernamePos);
        string username = line.substr(usernamePos, usernameEnd - usernamePos);

        userIdToName[id] = username;

        bool pauseAll = line.find("\"pauseAll\": \"true\"") != string::npos;
        bool likes = line.find("\"likes\": \"true\"") != string::npos;
        bool tags = line.find("\"tags\": \"true\"") != string::npos;
        bool comments = line.find("\"comments\": \"true\"") != string::npos;
        bool follows = line.find("\"newFollowers\": \"true\"") != string::npos;
        bool messages = line.find("\"messageRequests\": \"true\"") != string::npos;

        userPrefs[username] = {pauseAll, likes, tags, comments, follows, messages};
    }
}

void loadPosts(const string& path) {
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        size_t idPos = line.find("\"id\":\"") + 6;
        size_t idEnd = line.find("\"", idPos);
        string postId = line.substr(idPos, idEnd - idPos);

        size_t ownerPos = line.find("\"ownerUsername\":\"") + 18;
        size_t ownerEnd = line.find("\"", ownerPos);
        string owner = line.substr(ownerPos, ownerEnd - ownerPos);

        postToOwner[postId] = owner;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        cerr << "Usage: nynotifications.exe posts.json users.json events.txt output.txt username" << endl;
        return 1;
    }

    string postsPath = argv[1];
    string usersPath = argv[2];
    string eventsPath = argv[3];
    string outputPath = argv[4];
    string targetUser = argv[5];

    loadUsers(usersPath);
    loadPosts(postsPath);

    vector<unique_ptr<Notification>> notifications;

    ifstream eventsFile(eventsPath);
    string actor, type, target;
    while (eventsFile >> actor >> type >> target) {
        if (type == "likes") {
            if (postToOwner.find(target) != postToOwner.end()) {
                string owner = postToOwner[target];
                if (owner == targetUser && shouldNotify(owner, "likes"))
                    notifications.push_back(make_unique<LikeNotification>(actor));
            }
        } else if (type == "tags") {
            if (target == targetUser && shouldNotify(target, "tags"))
                notifications.push_back(make_unique<TagNotification>(actor));
        } else if (type == "comments_on") {
            if (postToOwner.find(target) != postToOwner.end()) {
                string owner = postToOwner[target];
                if (owner == targetUser && shouldNotify(owner, "comments"))
                    notifications.push_back(make_unique<CommentNotification>(actor));
            }
        } else if (type == "follows") {
            if (target == targetUser && shouldNotify(target, "follows"))
                notifications.push_back(make_unique<FollowNotification>(actor));
        } else if (type == "messageRequests") {
            if (target == targetUser && shouldNotify(target, "messages"))
                notifications.push_back(make_unique<MessageRequestNotification>(actor));
        }
    }

    ofstream outFile(outputPath);
    for (size_t i = 0; i < notifications.size(); ++i) {
        outFile << notifications[i]->format() << endl;
    }

    return 0;
}
