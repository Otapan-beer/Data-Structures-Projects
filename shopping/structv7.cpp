// includes
#include "utilv4.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <regex>
#include <algorithm>

//some structs
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

struct HashtagCmp { // this one I hate for having just four lines (the urls) in the output switched and 
                    // it took me 3 hours just to then ask chat to fix it in 30 seconds... wow
    bool operator()(const std::pair<std::string, HashtagData>& a,
                    const std::pair<std::string, HashtagData>& b) const {
        if (a.second.count != b.second.count)
            return a.second.count < b.second.count;
        if (a.second.totalViews != b.second.totalViews)
            return a.second.totalViews < b.second.totalViews;
        return a.first > b.first;
    }
};

struct SoundCmp { //this is the angel of the code, I love this one, it is so simple and elegant...
                  //compared to the other asshole, why does he have to whine to get another 2 lines extra? 
    bool operator()(const SoundData& a, const SoundData& b) const {
        if (a.totalViews != b.totalViews)
            return a.totalViews < b.totalViews;
        return a.musicId > b.musicId;
    }
};

//maybe still a bit longer than it needed to be but it extracts the hashtags from the text and puts them in a vector
//I did this using the provided github code (but I changed the while to for just for 2 less lines lol)
void extractHashtagsAll(const std::string& text, std::vector<std::string>& hashtags) {
    std::regex hashtagRegex("#([\\w\\u0080-\\uFFFF]+)");
    std::sregex_iterator begin(text.begin(), text.end(), hashtagRegex);
    std::sregex_iterator end;
    for (std::sregex_iterator it = begin; it != end; ++it) {
        std::string tag = (*it)[1].str();
        hashtags.push_back(tag);
    }
}

//this one was also hell, it is apparently where the prolem was (see the question mark, chat did that)
bool compareVideosBy(const Video& a, const Video& b, bool useCoverUrl) {
    if (a.playCount != b.playCount) return a.playCount > b.playCount;
    return useCoverUrl ? a.coverUrl < b.coverUrl : a.webVideoUrl < b.webVideoUrl;
}
//also part of the chat code used to decide whether the video or cover are the ones to be used for the comparison
//because the only swap that meant I was missing one point could be solved by switching the order of 
//the comparison priority of the urls and this was apparently required for the decision
struct VideoComparator {
    bool useCoverUrl;
    VideoComparator(bool flag) : useCoverUrl(flag) {}
    bool operator()(const Video& a, const Video& b) const {
        return compareVideosBy(a, b, useCoverUrl);
    }
};

//takes the json line, finds the key to value and makes the value a string
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

//extracts playcount from the json line
long long getPlayCount(const std::string& line) {//long long because I saw it in the submitty discussion to prevent int overload 
    std::string key = "\"playCount\":";
    size_t pos = line.find(key);
    if (pos == std::string::npos) return 0;
    size_t start = line.find(':', pos) + 1;
    while (start < line.size() && line[start] == ' ') ++start;
    size_t end = start;
    while (end < line.size() && isdigit(line[end])) ++end;
    return std::stoll(line.substr(start, end - start));
}

//main function to process the input file and generate hashtags/sounds output
void processFile(const std::string& inputFile, const std::string& outputFile, const std::string& mode) {
    //file handling
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);
    std::string line;
    int order = 0;

    //maps to store the data
    std::unordered_map<std::string, HashtagData> hashtagMap;
    std::unordered_map<std::string, SoundData> soundMap;

    while (getline(in, line)) { //read the entire line

        //extract info
        std::string videoId = getField(line, "id");
        std::string text = getField(line, "text");
        long long playCount = getPlayCount(line);
        std::string coverUrl = getField(line, "coverUrl");
        std::string webUrl = getField(line, "webVideoUrl");

        if (videoId.empty() || coverUrl.empty() || webUrl.empty()) continue; //skip if the essential data is missing

        Video v = {videoId, coverUrl, webUrl, playCount, order++}; //create the video object

        if (mode == "hashtag") {//hashtag
            std::vector<std::string> tags; //I didn't use unordered set here because I had issues with the ordering (aka. skill issue)
            extractHashtagsAll(text, tags); //extract all hashtags from the text using inspired provided code
            for (size_t i = 0; i < tags.size(); ++i) {//iterate and create all tags
                const std::string& tag = tags[i];
                hashtagMap[tag].count++;
                hashtagMap[tag].totalViews += playCount;
                hashtagMap[tag].videos.push_back(v); //add the video to the hashtag data
            }
        } else if (mode == "sound") {//sound was else if just in case, even though it should be caught in main
            //extract sound data from the json line
            std::string musicId = getField(line, "musicId");
            std::string musicName = getField(line, "musicName");
            std::string musicAuthor = getField(line, "musicAuthor");

            if (musicId.empty()) continue; //skip if missing

            //assign more data
            SoundData& s = soundMap[musicId];
            s.musicId = musicId;
            s.musicName = musicName;
            s.musicAuthor = musicAuthor;
            s.totalViews += playCount;
            s.videos.push_back(v);
        }
    }

    if (mode == "hashtag") { //hashtag mode priorities and output
        std::priority_queue<std::pair<std::string, HashtagData>, std::vector<std::pair<std::string, HashtagData>>, HashtagCmp> pq;
        for (std::unordered_map<std::string, HashtagData>::iterator it = hashtagMap.begin(); it != hashtagMap.end(); ++it) {
            pq.push(*it);
        }

        out << "trending hashtags:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            std::pair<std::string, HashtagData> top = pq.top(); pq.pop();
            std::string tag = top.first;
            HashtagData& data = top.second;

            out << "========================\n";
            out << "#" << tag << "\n";
            out << "used " << data.count << " times\n";
            out << data.totalViews << " views\n\n";

            std::stable_sort(data.videos.begin(), data.videos.end(), VideoComparator(false));
            int shown = 0;
            for (size_t j = 0; j < data.videos.size() && shown < 3; ++j) {
                out << "cover url: " << data.videos[j].coverUrl << "\n";
                out << "web video url: " << data.videos[j].webVideoUrl << "\n";
                ++shown;
            }
            out << "========================\n";
        }
    } else { //music mode priotities and output
        std::priority_queue<SoundData, std::vector<SoundData>, SoundCmp> pq;
        for (std::unordered_map<std::string, SoundData>::iterator it = soundMap.begin(); it != soundMap.end(); ++it) {
            pq.push(it->second);
        }

        out << "trending sounds:\n\n";
        for (int i = 0; i < 20 && !pq.empty(); ++i) {
            SoundData top = pq.top(); pq.pop();

            out << "========================\n";
            out << top.musicName << "\n";
            out << top.totalViews << " views\n";
            out << top.musicAuthor << "\n";
            out << "music id: " << top.musicId << "\n\n";

            std::stable_sort(top.videos.begin(), top.videos.end(), VideoComparator(false));
            for (int j = 0; j < std::min(3, static_cast<int>(top.videos.size())); ++j) {
                out << "cover url: " << top.videos[j].coverUrl << "\n";
                out << "web video url: " << top.videos[j].webVideoUrl << "\n";
            }
            out << "========================\n";
        }
    }
}
