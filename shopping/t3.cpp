#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <regex>
#include <algorithm>
#include "json.h"

using json = nlohmann::json;

struct Video {
    std::string id;
    std::string coverUrl;
    std::string webVideoUrl;
    long long playCount;
    int order;
};

struct HashtagData {
    int count = 0;
    long long totalViews = 0;
    std::vector<Video> videos;
};

struct SoundData {
    std::string musicName;
    std::string musicAuthor;
    std::string musicId;
    long long totalViews = 0;
    std::vector<Video> videos;
};

struct HashtagCmp {
    bool operator()(const std::pair<std::string, HashtagData>& a,
                    const std::pair<std::string, HashtagData>& b) const {
        if (a.second.count != b.second.count)
            return a.second.count < b.second.count;
        if (a.second.totalViews != b.second.totalViews)
            return a.second.totalViews < b.second.totalViews;
        return a.first > b.first;
    }
};

struct SoundCmp {
    bool operator()(const SoundData& a, const SoundData& b) const {
        if (a.totalViews != b.totalViews)
            return a.totalViews < b.totalViews;
        return a.musicId > b.musicId;
    }
};

void extractHashtagsAll(const std::string& text, std::vector<std::string>& hashtags) {
    std::regex hashtagRegex("#([\\w\\u0080-\\uFFFF]+)");
    std::sregex_iterator begin(text.begin(), text.end(), hashtagRegex);
    std::sregex_iterator end;
    for (std::sregex_iterator it = begin; it != end; ++it) {
        std::string tag = (*it)[1].str();
        hashtags.push_back(tag);
    }
}

bool compareVideos(const Video& a, const Video& b) {
    if (a.playCount != b.playCount) return a.playCount > b.playCount;
    if (a.order != b.order) return a.order < b.order;
    return a.id > b.id;
}

void processFile(const std::string& inputFile, const std::string& outputFile, const std::string& mode) {
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);
    std::string line;
    int order = 0;

    std::unordered_map<std::string, HashtagData> hashtagMap;
    std::unordered_map<std::string, SoundData> soundMap;

    while (getline(in, line)) {
        json j;
        try {
            j = json::parse(line);
        } catch (...) {
            std::cerr << "[DEBUG] Skipping invalid JSON line." << std::endl;
            continue;
        }

        std::string videoId = j.value("id", "");
        std::string text = j.value("text", "");
        long long playCount = j.value("playCount", 0LL);
        std::string coverUrl = j.value("coverUrl", "");
        std::string webUrl = j.value("webVideoUrl", "");

        if (videoId.empty() || coverUrl.empty() || webUrl.empty()) {
            std::cerr << "[DEBUG] Skipping video due to missing fields. ID: " << videoId << " text: " << text << " playCount: " << playCount << std::endl;
            continue;
        }

        Video v{videoId, coverUrl, webUrl, playCount, order++};

        if (mode == "hashtag") {
            std::vector<std::string> tags;
            extractHashtagsAll(text, tags);
            std::cerr << "[DEBUG] Video " << videoId << " uses hashtags: ";
            for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it) {
                std::cerr << "#" << *it << " ";
                hashtagMap[*it].count++;
                hashtagMap[*it].totalViews += playCount;
                hashtagMap[*it].videos.push_back(v);
            }
            std::cerr << "| playCount: " << playCount << std::endl;
        } else if (mode == "sound") {
            std::string musicId = j.value("musicId", "");
            std::string musicName = j.value("musicName", "");
            std::string musicAuthor = j.value("musicAuthor", "");

            if (musicId.empty()) {
                std::cerr << "[DEBUG] Skipping video due to missing music ID. Video ID: " << videoId << std::endl;
                continue;
            }

            SoundData& s = soundMap[musicId];
            s.musicId = musicId;
            s.musicName = musicName;
            s.musicAuthor = musicAuthor;
            s.totalViews += playCount;
            s.videos.push_back(v);
        }
    }

    if (mode == "hashtag") {
        std::priority_queue<std::pair<std::string, HashtagData>, std::vector<std::pair<std::string, HashtagData>>, HashtagCmp> pq;
        for (std::unordered_map<std::string, HashtagData>::iterator it = hashtagMap.begin(); it != hashtagMap.end(); ++it) {
            std::cerr << "[DEBUG] Queueing hashtag: #" << it->first << " used " << it->second.count << " times, views: " << it->second.totalViews << std::endl;
            pq.push(std::make_pair(it->first, it->second));
        }

        out << "trending hashtags:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            std::pair<std::string, HashtagData> top = pq.top(); pq.pop();
            std::string tag = top.first;
            HashtagData& data = top.second;

            std::cerr << "[DEBUG] Outputting hashtag: #" << tag << " used " << data.count << " times, views: " << data.totalViews << std::endl;
            out << "========================\n";
            out << "#" << tag << "\n";
            out << "used " << data.count << " times\n";
            out << data.totalViews << " views\n\n";

            std::sort(data.videos.begin(), data.videos.end(), compareVideos);
            for (int j = 0; j < std::min(3, (int)data.videos.size()); ++j) {
                out << "cover url: " << data.videos[j].coverUrl << "\n";
                out << "web video url: " << data.videos[j].webVideoUrl << "\n";
            }
            out << "========================\n";
        }
    } else {
        std::priority_queue<SoundData, std::vector<SoundData>, SoundCmp> pq;
        for (std::unordered_map<std::string, SoundData>::iterator it = soundMap.begin(); it != soundMap.end(); ++it) {
            std::cerr << "[DEBUG] Queueing sound: " << it->second.musicId << " with views: " << it->second.totalViews << std::endl;
            pq.push(it->second);
        }

        out << "trending sounds:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            SoundData top = pq.top(); pq.pop();

            std::cerr << "[DEBUG] Outputting sound: " << top.musicId << " views: " << top.totalViews << std::endl;
            out << "========================\n";
            out << top.musicName << "\n";
            out << top.totalViews << " views\n";
            out << top.musicAuthor << "\n";
            out << "music id: " << top.musicId << "\n\n";

            std::sort(top.videos.begin(), top.videos.end(), compareVideos);
            for (int j = 0; j < std::min(3, (int)top.videos.size()); ++j) {
                out << "cover url: " << top.videos[j].coverUrl << "\n";
                out << "web video url: " << top.videos[j].webVideoUrl << "\n";
            }
            out << "========================\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: nytrends.exe input.json output.txt hashtag|sound" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string mode = argv[3];

    if (mode != "hashtag" && mode != "sound") {
        std::cerr << "Invalid mode. Use 'hashtag' or 'sound'" << std::endl;
        return 1;
    }

    processFile(inputFile, outputFile, mode);
    return 0;
}
