#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "p1.h"

// Extracts: "key": "value"
std::string extract_string(const std::string& src, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*\"([^\"]*)\"";
    std::regex rgx(pattern);
    std::smatch match;
    if (std::regex_search(src, match, rgx)) {
        return match[1];
    }
    return "";
}

// Extracts: "key": 123.45
double extract_double(const std::string& src, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*([0-9.]+)";
    std::regex rgx(pattern);
    std::smatch match;
    if (std::regex_search(src, match, rgx)) {
        return std::stod(match[1]);
    }
    return 0.0;
}

// Extracts: "key": 123
int extract_int(const std::string& src, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*(\\d+)";
    std::regex rgx(pattern);
    std::smatch match;
    if (std::regex_search(src, match, rgx)) {
        return std::stoi(match[1]);
    }
    return 0;
}

// Gets first "thumb": "https://..." from the images array
std::string extract_first_thumb(const std::string& src) {
    std::regex rgx("\"thumb\"\\s*:\\s*\"(https://[^\"]+)\"");
    std::smatch match;
    if (std::regex_search(src, match, rgx)) {
        return match[1];
    }
    return "";
}

// Load products without json.hpp
std::vector<Product> load_products(const std::string& filename) {
    std::ifstream infile(filename);
    std::string line;
    std::vector<Product> products;

    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return products;
    }

    while (std::getline(infile, line)) {
        Product p;
        p.main_category = extract_string(line, "main_category");
        p.title = extract_string(line, "title");
        p.average_rating = extract_double(line, "average_rating");
        p.rating_number = extract_int(line, "rating_number");
        p.price = extract_double(line, "price");
        p.thumb_url = extract_first_thumb(line);

        if (!p.title.empty()) {
            products.push_back(p);
        }
    }

    return products;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./nyshopping input.json\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::vector<Product> products = load_products(input_file);

    std::cout << "Loaded " << products.size() << " products.\n\n";

    for (const auto& p : products) {
        p.print();
    }

    return 0;
}
