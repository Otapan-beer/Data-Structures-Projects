#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <regex>
#include <algorithm>

class SearchEngine {
public:
    void crawl(const std::string& seedFile);
    void processQueries(const std::string& inputFile);
    void rankAndOutputResults();

private:
    std::map<std::string, std::set<std::string>> invertedIndex;
    std::map<std::string, std::string> pageTitles;
    std::map<std::string, std::string> pageDescriptions;
    std::map<std::string, std::string> pageContent;
    std::map<std::string, std::set<std::string>> backlinks;
    std::set<std::string> visited;

    void parseHTML(const std::string& filePath);
    std::list<std::string> extractLinks(const std::string& fileContent);
    double computeKeywordDensity(const std::string& keyword, const std::string& document);
    double computeBacklinksScore(const std::string& document);
};

void SearchEngine::crawl(const std::string& seedFile) {
    if (visited.find(seedFile) != visited.end()) return;
    visited.insert(seedFile);
    parseHTML(seedFile);
}

void SearchEngine::parseHTML(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    std::regex titleRegex("<title>(.*?)</title>");
    std::smatch match;
    if (std::regex_search(content, match, titleRegex)) {
        pageTitles[filePath] = match[1].str();
    }
    
    std::regex descRegex("<meta name=\"description\" content=\"(.*?)\"/>");
    if (std::regex_search(content, match, descRegex)) {
        pageDescriptions[filePath] = match[1].str();
    }
    
    pageContent[filePath] = content;
    
    std::list<std::string> links = extractLinks(content);
    for (const std::string& link : links) {
        backlinks[link].insert(filePath);
        crawl(link);
    }
}

std::list<std::string> SearchEngine::extractLinks(const std::string& fileContent) {
    std::list<std::string> links;
    std::regex linkRegex("<a\\s+[^>]*href\\s*=\\s*['\"]([^'\"]+)['\"][^>]*>");
    std::smatch match;
    std::string::const_iterator start = fileContent.cbegin();
    while (std::regex_search(start, fileContent.cend(), match, linkRegex)) {
        if (match.size() > 1) {
            links.push_back(match[1].str());
        }
        start = match.suffix().first;
    }
    return links;
}

void SearchEngine::processQueries(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) return;
    std::string query;
    int queryNum = 1;
    while (getline(file, query)) {
        std::ofstream outFile("out" + std::to_string(queryNum++) + ".txt");
        // Process the query and output results here
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: nysearch.exe <seed_html_file> <input_queries_file>" << std::endl;
        return 1;
    }
    SearchEngine engine;
    engine.crawl(argv[1]);
    engine.processQueries(argv[2]);
    return 0;
}
