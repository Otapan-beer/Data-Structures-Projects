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

// Function to extract the title from HTML content
std::string extractTitle(const std::string& fileContent) {
    std::regex titleRegex("<title>(.*?)</title>");
    std::smatch match;
    if (std::regex_search(fileContent, match, titleRegex)) {
        return match[1];
    }
    return "";
}

// Function to extract the description from HTML content
std::string extractDescription(const std::string& fileContent) {
    std::regex descRegex("<meta\\s+name=\"description\"\\s+content=\"([^\"]*)\"");
    std::smatch match;
    if (std::regex_search(fileContent, match, descRegex)) {
        return match[1];
    }
    return "";
}

// Function to extract the body content from HTML
std::string extractBodyContent(const std::string& fileContent) {
    std::regex bodyRegex("<body[^>]*>(.*)</body>");
    std::smatch match;
    if (std::regex_search(fileContent, match, bodyRegex)) {
        return match[1];
    }
    return "";
}

// Function to construct a snippet from the body content
std::string constructSnippet(const std::string& bodyContent, const std::string& query) {
    // Find the first occurrence of the query in the body content
    size_t queryPos = bodyContent.find(query);
    if (queryPos == std::string::npos) {
        // If the query is not found, return the first 120 characters of the body
        return bodyContent.substr(0, 120);
    }

    // Find the start of the sentence containing the query
    size_t periodPos = bodyContent.rfind('.', queryPos);
    if (periodPos == std::string::npos) {
        periodPos = 0;
    } else {
        periodPos++; // Move past the period
    }

    // Skip any whitespace after the period
    while (periodPos < bodyContent.length() && std::isspace(bodyContent[periodPos])) {
        periodPos++;
    }

    // Extract 120 characters starting from the beginning of the sentence
    return bodyContent.substr(periodPos, 120);
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
        std::string fullPath;
        if (link[0] == '/') {
            fullPath = "html_files" + link;
        } else {
            size_t lastSlashPos = filePath.find_last_of('/');
            if (lastSlashPos != std::string::npos) {
                fullPath = filePath.substr(0, lastSlashPos + 1) + link;
            } else {
                fullPath = link;
            }
        }

        // Normalize path (remove redundant ./ or ../)
        size_t pos;
        while ((pos = fullPath.find("/./")) != std::string::npos) {
            fullPath.erase(pos, 2);
        }
        while ((pos = fullPath.find("/../")) != std::string::npos) {
            size_t prevSlashPos = fullPath.rfind('/', pos - 1);
            if (prevSlashPos != std::string::npos) {
                fullPath.erase(prevSlashPos, pos + 3 - prevSlashPos);
            }
        }

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
    outFile << "Matching documents: " << std::endl;
    outFile << std::endl;

    for (const auto& doc : sortedDocs) {
        // Only include documents with a non-zero page score
        if (doc.second > 0) {
            std::string title = extractTitle(documents.at(doc.first));
            std::string description = extractDescription(documents.at(doc.first));
            std::string bodyContent = extractBodyContent(documents.at(doc.first));
            std::string snippet = constructSnippet(bodyContent, query);

            outFile << "Title: " << title << std::endl;
            outFile << "URL: " << doc.first << std::endl;
            outFile << "Description: " << description << std::endl;
            outFile << "Snippet: " << snippet << std::endl;
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