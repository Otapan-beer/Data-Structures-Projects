#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <regex>
#include <algorithm>

void crawl(const std::string& url, std::set<std::string>& visited, std::map<std::string, std::set<std::string>>& graph) {
    if (visited.find(url) != visited.end()) {
        return;
    }
    visited.insert(url);

    // Simulate fetching the page content
    std::string content = "<html><body><a href=\"http://example.com/page1\">Page 1</a><a href=\"http://example.com/page2\">Page 2</a></body></html>";

    std::regex url_regex(R"((http[s]?://[^\s]+))");
    auto urls_begin = std::sregex_iterator(content.begin(), content.end(), url_regex);
    auto urls_end = std::sregex_iterator();

    for (std::sregex_iterator i = urls_begin; i != urls_end; ++i) {
        std::string found_url = i->str();
        graph[url].insert(found_url);
        crawl(found_url, visited, graph);
    }
}


std::ifstream fileStream(filePath);
if (fileStream.is_open()) {
	std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
	// suppose filePath is the string "html_files/subdir1/file3.html", then at this point, the string fileContent will be the full content of this file file3.html.
	// do something with fileContent
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cerr << "Usage: nysearch.exe <seed_html_file> <input_queries_file>" << std::endl;
        return 1;
    }
    
}