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
    return a.order < b.order;
}

std::string getField(const std::string& line, const std::string& key) {
    size_t keyPos = line.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";
    size_t colon = line.find(':', keyPos);
    if (colon == std::string::npos) return "";

    size_t start = colon + 1;
    while (start < line.size() && line[start] == ' ') ++start;

    if (line[start] == '"') {
        ++start;
        size_t end = line.find('"', start);
        return (end != std::string::npos) ? line.substr(start, end - start) : "";
    } else {
        size_t end = start;
        while (end < line.size() && line[end] != ',' && line[end] != '}') ++end;
        return line.substr(start, end - start);
    }
}

long long getPlayCount(const std::string& line) {
    std::string key = "\"playCount\":";
    size_t pos = line.find(key);
    if (pos == std::string::npos) return 0;
    size_t start = line.find(':', pos) + 1;
    while (start < line.size() && line[start] == ' ') ++start;
    size_t end = start;
    while (end < line.size() && isdigit(line[end])) ++end;
    return std::stoll(line.substr(start, end - start));
}

void processFile(const std::string& inputFile, const std::string& outputFile, const std::string& mode) {
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);
    std::string line;
    int order = 0;

    std::unordered_map<std::string, HashtagData> hashtagMap;
    std::unordered_map<std::string, SoundData> soundMap;

    while (getline(in, line)) {
        std::string videoId = getField(line, "id");
        std::string text = getField(line, "text");
        long long playCount = getPlayCount(line);
        std::string coverUrl = getField(line, "coverUrl");
        std::string webUrl = getField(line, "webVideoUrl");

        if (videoId.empty() || coverUrl.empty() || webUrl.empty()) continue;

        Video v{videoId, coverUrl, webUrl, playCount, order++};

        if (mode == "hashtag") {
            std::vector<std::string> tags;
            extractHashtagsAll(text, tags);
            for (size_t i = 0; i < tags.size(); ++i) {
                const std::string& tag = tags[i];
                hashtagMap[tag].count++;
                hashtagMap[tag].totalViews += playCount;
                hashtagMap[tag].videos.push_back(v);
            }
        } else if (mode == "sound") {
            std::string musicId = getField(line, "musicId");
            std::string musicName = getField(line, "musicName");
            std::string musicAuthor = getField(line, "musicAuthor");

            if (musicId.empty()) continue;

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
        for (const auto& pair : hashtagMap) {
            pq.push(pair);
        }

        out << "trending hashtags:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            auto top = pq.top(); pq.pop();
            std::string tag = top.first;
            HashtagData& data = top.second;

            out << "========================\n";
            out << "#" << tag << "\n";
            out << "used " << data.count << " times\n";
            out << data.totalViews << " views\n\n";

            std::stable_sort(data.videos.begin(), data.videos.end(), compareVideos);
            int shown = 0;
            for (size_t j = 0; j < data.videos.size() && shown < 3; ++j) {
                out << "cover url: " << data.videos[j].coverUrl << "\n";
                out << "web video url: " << data.videos[j].webVideoUrl << "\n";
                ++shown;
            }
            out << "========================\n";
        }
    } else {
        std::priority_queue<SoundData, std::vector<SoundData>, SoundCmp> pq;
        for (const auto& pair : soundMap) {
            pq.push(pair.second);
        }

        out << "trending sounds:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            SoundData top = pq.top(); pq.pop();

            out << "========================\n";
            out << top.musicName << "\n";
            out << top.totalViews << " views\n";
            out << top.musicAuthor << "\n";
            out << "music id: " << top.musicId << "\n\n";

            std::stable_sort(top.videos.begin(), top.videos.end(), compareVideos);
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
