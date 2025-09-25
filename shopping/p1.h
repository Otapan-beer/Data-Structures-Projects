#pragma once
#include <string>

struct Product {
    std::string main_category;
    std::string title;
    double average_rating = 0.0;
    int rating_number = 0;
    double price = 0.0;
    std::string thumb_url;

    void print() const;
};
