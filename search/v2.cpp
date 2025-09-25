#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <regex>
#include <algorithm>
#include <cmath>

// Function to extract links from HTML content
std::list<std::string> extractLinksFromHTML(const std::string& fileContent) {
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

// Function to read the entire content of a file into a string
std::string readFileContent(const std::string& filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return content;
}

// Recursive web crawler function
void crawl(const std::string& filePath, std::set<std::string>& visited, std::map<std::string, std::string>& documents) {
    if (visited.find(filePath) != visited.end()) {
        return;
    }
    visited.insert(filePath);

    std::string content = readFileContent(filePath);
    if (content.empty()) {
        return;
    }
    documents[filePath] = content;

    std::list<std::string> links = extractLinksFromHTML(content);
    for (const std::string& link : links) {
        std::string fullPath = filePath.substr(0, filePath.find_last_of('/') + 1) + link;
        crawl(fullPath, visited, documents);
    }
}

// Function to calculate keyword density score
double calculateKeywordDensity(const std::string& document, const std::string& keyword, const std::map<std::string, std::string>& documents) {
    size_t keywordCount = 0;
    size_t documentLength = document.length();
    size_t totalKeywordCount = 0;
    size_t totalLength = 0;

    for (const auto& doc : documents) {
        size_t count = 0;
        size_t pos = doc.second.find(keyword);
        while (pos != std::string::npos) {
            count++;
            pos = doc.second.find(keyword, pos + keyword.length());
        }
        totalKeywordCount += count;
        totalLength += doc.second.length();
    }

    double keywordDensityAcrossAllDocuments = static_cast<double>(totalKeywordCount) / totalLength;
    size_t pos = document.find(keyword);
    while (pos != std::string::npos) {
        keywordCount++;
        pos = document.find(keyword, pos + keyword.length());
    }

    return static_cast<double>(keywordCount) / (documentLength * keywordDensityAcrossAllDocuments);
}

// Function to calculate backlinks score
double calculateBacklinksScore(const std::string& filePath, const std::map<std::string, std::list<std::string>>& outgoingLinks) {
    double score = 0.0;
    for (const auto& link : outgoingLinks) {
        if (std::find(link.second.begin(), link.second.end(), filePath) != link.second.end()) {
            score += 1.0 / (1 + link.second.size());
        }
    }
    return score;
}

// Function to search and rank documents
void searchAndRank(const std::string& query, const std::map<std::string, std::string>& documents, const std::map<std::string, std::list<std::string>>& outgoingLinks, std::ofstream& outFile) {
    std::map<std::string, double> pageScores;

    for (const auto& doc : documents) {
        double keywordDensityScore = 0.0;
        std::istringstream iss(query);
        std::string keyword;
        while (iss >> keyword) {
            keywordDensityScore += calculateKeywordDensity(doc.second, keyword, documents);
        }

        double backlinksScore = calculateBacklinksScore(doc.first, outgoingLinks);
        double pageScore = 0.5 * keywordDensityScore + 0.5 * backlinksScore;
        pageScores[doc.first] = pageScore;
    }

    // Sort documents by page score in descending order
    std::vector<std::pair<std::string, double>> sortedDocs(pageScores.begin(), pageScores.end());
    std::sort(sortedDocs.begin(), sortedDocs.end(), [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
        return a.second > b.second;
    });

    // Output the results
    if (sortedDocs.empty()) {
        outFile << "Your search - " << query << " - did not match any documents." << std::endl;
    } else {
        for (const auto& doc : sortedDocs) {
            outFile << "Title: " << "Extracted Title" << std::endl; // Extract title from HTML
            outFile << "URL: " << doc.first << std::endl;
            outFile << "Description: " << "Extracted Description" << std::endl; // Extract description from HTML
            outFile << "Snippet: " << "Extracted Snippet" << std::endl; // Extract snippet from HTML
            outFile << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <seed_url> <input_file>" << std::endl;
        return 1;
    }

    std::string seedUrl = argv[1];
    std::string inputFile = argv[2];

    std::set<std::string> visited;
    std::map<std::string, std::string> documents;
    std::map<std::string, std::list<std::string>> outgoingLinks;

    // Crawl the web starting from the seed URL
    crawl(seedUrl, visited, documents);

    // Read search queries from input file
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return 1;
    }

    std::string query;
    int queryCount = 1;
    while (std::getline(inFile, query)) {
        std::string outFileName = "out" + std::to_string(queryCount) + ".txt";
        std::ofstream outFile(outFileName);
        if (!outFile.is_open()) {
            std::cerr << "Failed to create output file: " << outFileName << std::endl;
            continue;
        }

        searchAndRank(query, documents, outgoingLinks, outFile);
        outFile.close();
        queryCount++;
    }

    inFile.close();
    return 0;
}