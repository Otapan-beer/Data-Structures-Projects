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
    std::list<std::string> links; // List to store extracted links
    std::regex linkRegex("<a\\s+[^>]*href\\s*=\\s*['\"]([^'\"]+)['\"][^>]*>"); // Regex to match <a> tags with href attributes
    std::smatch match; // Match object to store regex results

    // Iterate through the file content to find all links
    std::string::const_iterator start = fileContent.cbegin();
    while (std::regex_search(start, fileContent.cend(), match, linkRegex)) {
        if (match.size() > 1) {
            links.push_back(match[1].str()); // Add the extracted link to the list
        }
        start = match.suffix().first; // Move to the next position in the file content
    }

    return links; // Return the list of links
}

// Function to extract the title from HTML content
std::string extractTitle(const std::string& fileContent) {
    std::regex titleRegex("<title>(.*?)</title>"); // Regex to match the <title> tag
    std::smatch match;
    if (std::regex_search(fileContent, match, titleRegex)) {
        return match[1]; // Return the title content
    }
    return ""; // Return an empty string if no title is found
}

// Function to extract the description from HTML content
std::string extractDescription(const std::string& fileContent) {
    std::regex descRegex("<meta\\s+name=\"description\"\\s+content=\"([^\"]*)\""); // Regex to match the meta description tag
    std::smatch match;
    if (std::regex_search(fileContent, match, descRegex)) {
        return match[1]; // Return the description content
    }
    return ""; // Return an empty string if no description is found
}

// Function to extract the body content from HTML
std::string extractBodyContent(const std::string& fileContent) {
    std::regex bodyRegex("<body[^>]*>(.*)</body>"); // Regex to match the <body> tag
    std::smatch match;
    if (std::regex_search(fileContent, match, bodyRegex)) {
        // Debug output: Print the extracted body content to console
        std::cout << "Extracted Body Content: " << match[1] << std::endl;
        return match[1]; // Return the body content
    } else {
        std::cout << "No <body> content found!" << std::endl; // Debugging statement
    }
    return ""; // Return an empty string if no body content is found
}


std::string extractSnippet(const std::string& bodyContent, const std::string& query) {
    // Remove links from the body content
    std::string snip = "";
    if(bodyContent.find(query) != std::string::npos) {
        snip = bodyContent.substr(bodyContent.find(query), 120);
    }
    else{
        std::istringstream iss(query);
        std::string word;
        while (iss >> word) {
            if (bodyContent.find(word) != std::string::npos) {
                snip = bodyContent.substr(bodyContent.find(word), 120);
                break;
            }
        }
    }
    std::cout << "Snippet: " << snip << std::endl;
    return snip; // Return the snippet
}



// Function to remove links from the body content
std::string removeLinks(const std::string& bodyContent) {
    // Remove all <a> tags (links) from the body content
    std::string cleanedContent = std::regex_replace(bodyContent, std::regex("<a\\s+[^>]*>.*?</a>"), "");
    return cleanedContent; // Return the cleaned content
}

// Function to construct a snippet from the body content
std::string constructSnippet(const std::string& bodyContent) {
    // Remove links from the body content
    std::string cleanedContent = removeLinks(bodyContent);

    // Debug output: Print the cleaned content to console
    std::cout << "Cleaned Content: " << cleanedContent << std::endl;

    // Remove HTML tags
    std::string plainText = std::regex_replace(cleanedContent, std::regex("<[^>]*>"), "");

    // Debug output: Print the plain text to console
    std::cout << "Plain Text: " << plainText << std::endl;

    // Extract the first 120 characters
    std::string snippet = plainText.substr(0, 120);

    // Debug output: Print the snippet to console
    std::cout << "Snippet: " << snippet << std::endl;

    return snippet; // Return the snippet
}

// Function to read the entire content of a file into a string
std::string readFileContent(const std::string& filePath) {
    std::ifstream fileStream(filePath); // Open the file
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl; // Print an error message if the file cannot be opened
        return "";
    }
    // Read the entire file content into a string
    std::string content((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

    // Debug output: Print the file path and content length to console
    std::cout << "Read file: " << filePath << " (Length: " << content.length() << ")" << std::endl;

    return content; // Return the file content
}

// Recursive web crawler function
void crawl(const std::string& filePath, std::set<std::string>& visited, std::map<std::string, std::string>& documents) {
    // Skip if the file has already been visited
    if (visited.find(filePath) != visited.end()) {
        return;
    }
    visited.insert(filePath); // Mark the file as visited

    // Read the file content
    std::string content = readFileContent(filePath);
    if (content.empty()) {
        return; // Skip if the file is empty
    }
    documents[filePath] = content; // Store the file content in the documents map

    // Extract links from the file content
    std::list<std::string> links = extractLinksFromHTML(content);
    for (const std::string& link : links) {
        std::string fullPath;
        if (link[0] == '/') {
            // Handle absolute paths (relative to html_files folder)
            fullPath = "html_files" + link;
        } else {
            // Handle relative paths
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
            fullPath.erase(pos, 2); // Remove /./
        }
        while ((pos = fullPath.find("/../")) != std::string::npos) {
            size_t prevSlashPos = fullPath.rfind('/', pos - 1);
            if (prevSlashPos != std::string::npos) {
                fullPath.erase(prevSlashPos, pos + 3 - prevSlashPos); // Remove /../
            }
        }

        // Recursively crawl the new file
        crawl(fullPath, visited, documents);
    }
}

// Function to calculate keyword density score
double calculateKeywordDensity(const std::string& document, const std::string& keyword, const std::map<std::string, std::string>& documents) {
    size_t keywordCount = 0; // Number of times the keyword appears in the document
    size_t documentLength = document.length(); // Length of the document
    size_t totalKeywordCount = 0; // Total number of times the keyword appears in all documents
    size_t totalLength = 0; // Total length of all documents

    // Calculate total keyword count and total length across all documents
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

    // Calculate keyword density across all documents
    double keywordDensityAcrossAllDocuments = static_cast<double>(totalKeywordCount) / totalLength;

    // Calculate keyword density for the current document
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
    // Calculate the backlinks score based on the number and quality of incoming links
    for (const auto& link : outgoingLinks) {
        if (std::find(link.second.begin(), link.second.end(), filePath) != link.second.end()) {
            score += 1.0 / (1 + link.second.size());
        }
    }
    return score; // Return the backlinks score
}

// Function to search and rank documents
void searchAndRank(const std::string& query, const std::map<std::string, std::string>& documents, const std::map<std::string, std::list<std::string>>& outgoingLinks, std::ofstream& outFile) {
    std::vector<std::pair<double, std::string>> sortedDocs; // Vector to store documents and their scores

    // Calculate page scores for each document
    for (const auto& doc : documents) {
        double keywordDensityScore = 0.0;
        std::istringstream iss(query);
        std::string keyword;
        while (iss >> keyword) {
            keywordDensityScore += calculateKeywordDensity(doc.second, keyword, documents);
        }

        double backlinksScore = calculateBacklinksScore(doc.first, outgoingLinks);
        double pageScore = 0.5 * keywordDensityScore + 0.5 * backlinksScore; // Calculate the overall page score

        // Insert into vector (score as key, file path as value)
        if (pageScore > 0) {
            sortedDocs.push_back({pageScore, doc.first});
        }
    }

    // Sort by score in descending order
    std::sort(sortedDocs.begin(), sortedDocs.end(), [](const std::pair<double, std::string>& a, const std::pair<double, std::string>& b) {
        return a.first > b.first; // Sort by score
    });

    // Debugger output: Print scores for each document to console - why is this shit not working wtf
    std::cout << "Debugger Output - Scores for Query: " << query << std::endl;
    for (const auto& doc : sortedDocs) {
        std::cout << "Score: " << doc.first << ", URL: " << doc.second << std::endl;
    }
    std::cout << std::endl;

    // Output the results to the file
    outFile << "Matching documents: " << std::endl;
    outFile << std::endl;

    for (const auto& doc : sortedDocs) {
        std::string title = extractTitle(documents.at(doc.second));
        std::string description = extractDescription(documents.at(doc.second));
        std::string bodyContent = extractBodyContent(documents.at(doc.second));
        std::string snippet = extractSnippet(bodyContent,query);

        outFile << "Title: " << title << std::endl;
        outFile << "URL: " << doc.second << std::endl;
        outFile << "Description: " << description << std::endl;
        outFile << "Snippet: " << snippet << std::endl;
        outFile << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Program started!" << std::endl; // Debugging statement

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <seed_url> <input_file>" << std::endl;
        return 1;
    }

    std::string seedUrl = argv[1]; // Seed URL (starting point for crawling)
    std::string inputFile = argv[2]; // Input file containing search queries

    std::set<std::string> visited; // Set to keep track of visited files
    std::map<std::string, std::string> documents; // Map to store file paths and their content
    std::map<std::string, std::list<std::string>> outgoingLinks; // Map to store outgoing links for each file

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
        std::string outFileName = "out" + std::to_string(queryCount) + ".txt"; // Create output file name
        std::ofstream outFile(outFileName);
        if (!outFile.is_open()) {
            std::cerr << "Failed to create output file: " << outFileName << std::endl;
            continue;
        }

        // Search and rank documents for the current query
        searchAndRank(query, documents, outgoingLinks, outFile);
        outFile.close();
        queryCount++;
    }

    inFile.close();
    return 0;
}