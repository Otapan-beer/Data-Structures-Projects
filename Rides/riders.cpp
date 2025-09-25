#include <iostream> // read/write
#include <fstream> //in/out files
#include <string> //strings
#include <vector> //vectors
#include <filesystem> //creating files
#include <sstream> //string streams
#include <cmath> //for calculating distance

#include "riders.h" //include header

    // Constructor
    Rider::Rider(){}
    Rider::Rider(const std::string& cfirst, const std::string& clast, const std::string& cgender, int cage, 
        const std::string& cphone, double crating, const std::string& clocation, double cx, double cy, 
        const std::string& cdestination, double cdx, double cdy, const std::string& cvehicle, 
        std::string cstatus, std::string cDFirst, std::string cDLast, std::string cDPhone) {
            first = cfirst; last = clast; gender = cgender; age = cage; phone = cphone; rating = crating;
            location = clocation; x = cx; y = cy; destination = cdestination; dx = cdx; dy = cdy; 
            vehicle = cvehicle; status = cstatus; DFirst = cDFirst; DLast = cDLast; DPhone = cDPhone;
        }

    // Getters
    std::string Rider::getFirst() const {
        return first;
    }
    std::string Rider::getLast() const {
        return last;
    }
    std::string Rider::getGender() const {
        return gender;
    }
    int Rider::getAge() const {
        return age;
    }
    std::string Rider::getPhone() const {
        return phone;
    }
    double Rider::getRating() const {
        return rating;
    }
    double Rider::getX() const {
        return x;
    }
    double Rider::getY() const {
        return y;
    }
    std::string Rider::getLocation() const {
        return location;
    }
    std::string Rider::getDestination() const {
        return destination;
    }
    double Rider::getDX() const {
        return dx;
    }
    double Rider::getDY() const {
        return dy;
    }
    std::string Rider::getVehicle() const {
        return vehicle;
    }
    std::string Rider::getStatus() const {
        return status;
    }
    std::string Rider::getDriverFirst() const {
        return DFirst;
    }
    std::string Rider::getDriverLast() const {
        return DLast;
    }
    std::string Rider::getDriverPhone() const {
        return DPhone;
    }

    // Setters
    void Rider::setRiderStatus(const std::string& newStatus) {
        status = newStatus;
    }

    void Rider::setRiderPos(const double& newx, const double& newy) {
        x = newx;
        y = newy;
    }
    void Rider::setDriverPos(const double& newdx, const double& newdy) {
        dx = newdx;
        dy = newdy;
    }
    void Rider::setDriver( std::string newDFirst,  std::string newDLast,  std::string newDPhone) {
        DFirst = newDFirst;
        DLast = newDLast;
        DPhone = newDPhone;
    }