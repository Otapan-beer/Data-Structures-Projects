#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath> // for fmod to calculate stars

#include "business.h"

//provided functions
std::string getName(std::string& line){
    int start = 0;
    // starting from the position start, and search the string variable line,
    // to find the first name string.
    int key_start = line.find("name", start);
    int value_start = key_start + 7;
    // remember to use escape.
    int value_end = line.find("\"", value_start);
    int len = value_end - value_start;
    // go from start to end, but exclude the character at end.
    // when we use the substr(start, length) function on a std::string,
    // the substring includes the character at the start position,
    // and the length of the substring is length.
    // It does not include the character at the position start + length.
    std::string name = line.substr(value_start, len);
    return name;
}

std::string getCategories(std::string& line){
    int start = 0;
    // starting from the position start, and search the string variable line,
    // to find the first categories string.
    int key_start = line.find("categories", start);
    int value_start = key_start + 13;
    // remember to use escape.
    int value_end = line.find("\"", value_start);
    int len = value_end - value_start;
    // go from start to end, but exclude the character at end.
    // when we use the substr(start, length) function on a std::string,
    // the substring includes the character at the start position,
    // and the length of the substring is length.
    // It does not include the character at the position start + length.
    std::string categories = line.substr(value_start, len);
    return categories;
}

double getRating(std::string& line){
    int start = 0;
    // starting from the position start, and search the string variable line,
    // to find the first stars string.
    int key_start = line.find("stars", start);
    int value_start = key_start + 7;
    // remember to use escape.
    int value_end = line.find("\"", value_start);
    // -1 here because this is not a string.
    int len = value_end - value_start - 1;
    // go from start to end, but exclude the character at end.
    // when we use the substr(start, length) function on a std::string,
    // the substring includes the character at the start position,
    // and the length of the substring is length.
    // It does not include the character at the position start + length.
    // convert this string to a double
    double stars = stod(line.substr(value_start, len));
    return stars;
}

int getPrice(std::string& line) {
    int start = 0;
    int key_start = line.find("RestaurantsPriceRange2", start);
    if (key_start == std::string::npos) {
        return -1; // Return -1 if the field is missing
    }
    int value_start = key_start + 25;
    int value_end = line.find("\"", value_start);
    
    if (value_end == std::string::npos || value_start >= value_end) {
        return -1; // Invalid data format
    }
    
    std::string priceStr = line.substr(value_start, value_end - value_start);

    try {
        return std::stoi(priceStr); // Convert to int
    } catch (const std::invalid_argument& e) {
        return -1; // Handle invalid number format
    } catch (const std::out_of_range& e) {
        return -1; // Handle out-of-range error
    }
}



//added helper functions
int getReviewCount(std::string& line) {
    int start = 0;
    int key_start = line.find("review_count", start);
    if (key_start == std::string::npos) {
        return -1;
    }
    int value_start = key_start + 14;
    int value_end = line.find(",", value_start);
    if (value_end == std::string::npos || value_start >= value_end) {
        return -1;
    }
    
    std::string reviewStr = line.substr(value_start, value_end - value_start);
    
    try {
        return std::stoi(reviewStr);
    } catch (...) {
        return -1;
    }
}


std::string getCity(std::string& line) {
    int start = 0;
    int key_start = line.find("city", start);
    if (key_start == std::string::npos) {
        return "Unknown"; // Return "Unknown" if the field is not found
    }
    int value_start = key_start + 7;
    int value_end = line.find("\"", value_start);
    int len = value_end - value_start;

    return line.substr(value_start, len);
}

std::string getZipcode(std::string& line) {
    int start = 0;
    int key_start = line.find("postal_code", start);
    if (key_start == std::string::npos) {
        return ""; // Return an empty string if the field is not found
    }
    int value_start = key_start + 14;
    int value_end = line.find("\"", value_start);
    int len = value_end - value_start;

    return line.substr(value_start, len);
}



int main(int argc, char* argv[]) {

    if (argc < 5) { //just to be safe
        std::cerr << "Usage: " << argv[0] << " <input.json> <output.txt> <zipcode> <categories>\n";
        return 1;
    }

    std::list<std::string> targetCategories;

    std::string inputFile = std::string(argv[1]);
    std::string outputFile = std::string(argv[2]);
    std::string targetZip = std::string(argv[3]);
    for(int i = 4; i < argc; i++){
        targetCategories.push_back(std::string(argv[i]));
    }
    
    // Debugging Output (Optional)
    // std::cout << "Input File: " << inputFile << "\n";
    // std::cout << "Output File: " << outputFile << "\n";
    // std::cout << "Target Zipcode: " << targetZip << "\n";
    // std::cout << "Categories: ";
    // for (const auto& cat : targetCategories) {
    //     std::cout << cat << " ";
    // }
    // std::cout << "\n";


    std::ifstream file(inputFile);
    std::string line;
    std::list<Business> businesses;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::string name = getName(line);
            std::string categories = getCategories(line);
            double rating = getRating(line);
            int price = getPrice(line);
            int reviewCount = getReviewCount(line);
            std::string city = getCity(line);
            std::string zipcode = getZipcode(line);
    
            //filter by zipcode
            if (zipcode != targetZip) {
                continue; //go to the next restaurant
            }
    
            //convert categories string to a list of individual categories
            
            std::istringstream categoryStream(categories);
            std::string category;
            bool categoryMatch = false;
            
            while (std::getline(categoryStream, category, ',')) { // Assume categories are comma-separated
    category.erase(0, category.find_first_not_of(" \"")); // Trim whitespace/quotes
    category.erase(category.find_last_not_of(" \"") + 1);

    for (const std::string& targetCategory : targetCategories) {
        if (category.find(targetCategory) != std::string::npos) { 
            // If the target category is a substring of the business category
            categoryMatch = true;
            break;
        }
    }
    if (categoryMatch) break;
            }
    
            if (!categoryMatch) {
                continue; // Skip businesses that don't match any category
            }
    
            // Create and add the Business object if it meets the conditions
            Business b(name, rating, reviewCount, city, zipcode, price, categories);
            businesses.push_back(b);
        }
        file.close(); //we do not need it anymore
        std::ofstream outFile(outputFile); //open the output file
        if (!outFile) { // error check
            std::cerr << "Error: Could not open output file.\n";
        }
        else{ //opened just fine, we are outputing then
            if (businesses.size() != 0){ //if we have results
                businesses.sort([](const Business& a, const Business& b) { // some super weird function on sorting list by a double in a class
                    return a.getRating() > b.getRating(); //this should give it a sign that we want to sort just by rating...
                }); //basically from my understanding it will work a bit like bubblesort, where it checks 2 functions together and 
                //gets a bool response where if true the two swap until the whole list is sorted... or something like that
                outFile << "=====================\n";
                int counter = 1; // for number before the name
                for (const Business& b : businesses) {
                    outFile << counter << ". " << b.getName() << "\n";
                    double stars = b.getRating();
                    stars = fmod(stars, 1);
                    if (stars == 0.5){ //has a half star
                        int inting = int(b.getRating());
                        for (int i = 0; i < inting; i++){
                            outFile << "\u2605"; //full stars
                        }
                        outFile << "\u00BD" << " " << b.getRating() << " (" << b.getReviewCount() << " reviews)\n"; //half star
                    }
                    else{ //only full stars
                        for (int i = 0; i < b.getRating(); i++){
                            outFile << "\u2605"; //full stars
                        }
                        outFile << " " << b.getRating() << " (" << b.getReviewCount() << " reviews)\n";
                    }
                    outFile << b.getCity() << " ";
                    if (b.getPrice() != -1){
                        for (int i = 0; i < b.getPrice(); i++) {
                            outFile << "$"; // print the price if there are data for it
                        }
                    }
                    outFile << "\n";
                    outFile << b.getCategories() <<"\n";
                    outFile << "=====================\n";
                    counter ++;
                }
            }
            else{ //no results
                outFile<< "Sorry, we couldn't find any results\n";
            }
        }
    }
    else {
        std::cerr << "Unable to open file\n";
    }
}