#include <iostream>  // Input/output stream
#include <fstream>   // File stream
#include <sstream>
#include <vector>     // For dynamic arrays
#include <unordered_map> // For efficient key-value lookup
#include <memory>     // For smart pointers
#include <string>
#include <algorithm>  // For sorting

using namespace std;

// --- Notification Base Class ---
// Abstract class that all notification types inherit from
class Notification {
public:
    virtual string format() const = 0;  // Pure virtual function
    virtual ~Notification() {}
};

// --- Derived Notification Types ---
// Each of these classes formats a specific notification type
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
// Holds boolean values for each notification type
struct UserPreferences {
    bool pauseAll, likes, tags, comments, follows, messages;
};

// Associates a notification with its event line order
struct OrderedNotification {
    int order;
    unique_ptr<Notification> notif;

    OrderedNotification(int o, unique_ptr<Notification> n)
        : order(o), notif(std::move(n)) {}
};

// Global maps for quick lookup
unordered_map<string, string> postToOwner;
unordered_map<string, UserPreferences> userPrefs;
unordered_map<string, string> userIdToName;

// Determine if a user should be notified of a given event type
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

// --- Load Users ---
// Parses the users JSON file and fills userPrefs and userIdToName
void loadUsers(const string& path) {
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        size_t idPos = line.find("\"id\": \"");
        if (idPos == string::npos) continue;
        idPos += 7;
        size_t idEnd = line.find("\"", idPos);
        string id = line.substr(idPos, idEnd - idPos);

        size_t usernamePos = line.find("\"username\": \"", idEnd);
        if (usernamePos == string::npos) continue;
        usernamePos += 13;
        size_t usernameEnd = line.find("\"", usernamePos);
        string username = line.substr(usernamePos, usernameEnd - usernamePos);

        userIdToName[id] = username;

        // Parse notification preferences
        bool pauseAll = line.find("\"pauseAll\": \"true\"") != string::npos;
        bool likes = line.find("\"likes\": \"true\"") != string::npos;
        bool tags = line.find("\"tags\": \"true\"") != string::npos;
        bool comments = line.find("\"comments\": \"true\"") != string::npos;
        bool follows = line.find("\"newFollowers\": \"true\"") != string::npos;
        bool messages = line.find("\"messageRequests\": \"true\"") != string::npos;

        userPrefs[username] = {pauseAll, likes, tags, comments, follows, messages};
    }
}

// --- Load Posts ---
// Parses posts JSON to associate post IDs with their owners
void loadPosts(const string& path) {
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        size_t idKey = line.find("\"id\":\"");
        size_t ownerKey = line.find("\"ownerUsername\":\"");

        if (idKey == string::npos || ownerKey == string::npos)
            continue;

        size_t idStart = idKey + 6;
        size_t idEnd = line.find("\"", idStart);
        string postId = line.substr(idStart, idEnd - idStart);

        size_t ownerStart = ownerKey + 18;
        size_t ownerEnd = line.find("\"", ownerStart);
        string owner = line.substr(ownerStart, ownerEnd - ownerStart);

        postToOwner[postId] = owner;
    }
}

// --- Main Function ---
int main(int argc, char* argv[]) {
    // Expect exactly 5 arguments
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

    vector<OrderedNotification> notifications;

    ifstream eventsFile(eventsPath);
    string actor, type, target;
    int lineIndex = 0; // Keep track of event line order
    while (eventsFile >> actor >> type >> target) {
        // Process each type of event
        if (type == "likes") {
            if (postToOwner.find(target) != postToOwner.end()) {
                string owner = postToOwner[target];
                if (owner == targetUser && shouldNotify(owner, "likes")) {
                    notifications.push_back(OrderedNotification(lineIndex, unique_ptr<Notification>(new LikeNotification(actor))));
                }
            }
        } else if (type == "tags") {
            if (target == targetUser && shouldNotify(target, "tags")) {
                notifications.push_back(OrderedNotification(lineIndex, unique_ptr<Notification>(new TagNotification(actor))));
            }
        } else if (type == "comments_on") {
            if (postToOwner.find(target) != postToOwner.end()) {
                string owner = postToOwner[target];
                if (owner == targetUser && shouldNotify(owner, "comments")) {
                    notifications.push_back(OrderedNotification(lineIndex, unique_ptr<Notification>(new CommentNotification(actor))));
                }
            }
        } else if (type == "follows") {
            if (target == targetUser && shouldNotify(target, "follows")) {
                notifications.push_back(OrderedNotification(lineIndex, unique_ptr<Notification>(new FollowNotification(actor))));
            }
        } else if (type == "messageRequests") {
            if (target == targetUser && shouldNotify(target, "messages")) {
                notifications.push_back(OrderedNotification(lineIndex, unique_ptr<Notification>(new MessageRequestNotification(actor))));
            }
        }
        lineIndex++; // Increment to maintain original order
    }

    // Sort notifications by original order from events.txt
    sort(notifications.begin(), notifications.end(), [](const OrderedNotification& a, const OrderedNotification& b) {
        return a.order < b.order;
    });

    // Output notifications to file
    ofstream outFile(outputPath);
    for (size_t i = 0; i < notifications.size(); ++i) {
        outFile << notifications[i].notif->format() << endl;
    }

    return 0;
}
     