#include "business.h"
#include <sstream>

// Constructor
Business::Business(std::string name, double rating, int reviewCount, std::string city, 
                   std::string zipcode, int price, std::string categories)
    : name(name), rating(rating), reviewCount(reviewCount), city(city), 
      zipcode(zipcode), price(price), categories(categories) {}

// Getters
std::string Business::getName() const {
    return name;
}
double Business::getRating() const {
    return rating;
}
int Business::getReviewCount() const {
    return reviewCount;
}
std::string Business::getCity() const {
    return city;
}
std::string Business::getZipcode() const {
    return zipcode;
}
int Business::getPrice() const {
    return price;
}
std::string Business::getCategories() const {
    return categories;
}

// Function to return business details as a string
std::string Business::toString() const {
    std::ostringstream oss;
    oss << "Name: " << name << "\n"
        << "Rating: " << rating << "\n"
        << "Reviews: " << reviewCount << "\n"
        << "City: " << city << "\n"
        << "Zipcode: " << zipcode << "\n"
        << "Price: " << price << "\n"
        << "Categories: " << categories << "\n";
    return oss.str();
}
