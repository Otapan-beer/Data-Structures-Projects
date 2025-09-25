#include <iostream> // read/write
#include <fstream> //in/out files
#include <string> //strings
#include <vector> //vectors
#include <filesystem> //copied this one
#include <sstream> //string stream
#include <cmath> //calculating

#include "drivers.h" //include header

// Constructor
Driver::Driver(){}
Driver::Driver(const std::string& cfirst, const std::string& clast, const std::string& cgender, 
    const int& cage, const std::string& cphone, const double& crating, double cx, double cy, 
    const std::string& cvehicle, std::string cstatus, 
    std::string cRFirst, std::string cRLast, std::string cRPhone) {
    first = cfirst; last = clast; gender = cgender; age = cage; phone = cphone; rating = crating;
    x = cx; y = cy; vehicle = cvehicle; status = cstatus; RFirst = cRFirst; RLast = cRLast; RPhone = cRPhone;
    }

// Getters
std::string Driver::getFirstName() {
    return first;
}
std::string Driver::getLastName() {
    return last;
}
std::string Driver::getGender() const {
    return gender;
}
int Driver::getAge() const {
    return age;
}
std::string Driver::getPhone() {
    return phone;
}
double Driver::getRating() const {
    return rating;
}
double Driver::getX() const {
    return x;
}
double Driver::getY() const {
    return y;
}
std::string Driver::getVehicle() const {
    return vehicle;
}
std::string Driver::getStatus() const {
    return status;
}
std::string Driver::getRiderFirstName() const {
    return RFirst;
}
std::string Driver::getRiderLastName() const {
    return RLast;
}
std::string Driver::getRiderPhone() const {
    return RPhone;
}

// Setters
void Driver::setDriverStatus(const std::string& newStatus) {
    status = newStatus;
}
void Driver::setRider(const std::string& newRFirst, const std::string& newRLast, const std::string& newRPhone) {
    RFirst = newRFirst;
    RLast = newRLast;
    RPhone = newRPhone;
}
void Driver::setDriverPos(const double& newx, const double& newy) {
    x = newx;
    y = newy;
}


