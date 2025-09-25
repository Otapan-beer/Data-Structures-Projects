#ifndef BUSINESS_H
#define BUSINESS_H

#include <string>

class Business {
private:
    std::string name;
    double rating;
    int reviewCount;
    std::string city;
    std::string zipcode;
    int price;
    std::string categories;

public:
    // Constructor
    Business(std::string name, double rating, int reviewCount, std::string city, 
            std::string zipcode, int price, std::string categories);

    // Getters
    std::string getName() const;
    double getRating() const;
    int getReviewCount() const;
    std::string getCity() const;
    std::string getZipcode() const;
    int getPrice() const;
    std::string getCategories() const;

    // Function to print business details
    std::string toString() const;
};

#endif
