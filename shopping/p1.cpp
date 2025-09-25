#include "p1.h"
#include <iostream>

void Product::print() const {
    std::cout << "Title: " << title << "\n";
    std::cout << "Rating: " << average_rating << "\n";
    std::cout << "Rating Count: " << rating_number << "\n";
    std::cout << "Thumbnail: " << thumb_url << "\n";
    std::cout << "Price: " << price << "\n";
    std::cout << "Main Category: " << main_category << "\n";
    std::cout << "-------------------------\n";
}
