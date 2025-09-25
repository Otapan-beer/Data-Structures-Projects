#include <iostream> // read/write
#include <fstream> //in/out files
#include <string> //strings
#include <vector> //vectors
#include <filesystem> //creating files
#include <sstream> //string streams
#include <cmath> //for calculating distance

#include "riders.h"

// Constructor
    Rider::Rider(const std::string& first, const std::string& last, const std::string& gender, int age, 
          const std::string& phone, double rating, const std::string& location, double x, double y, 
          const std::string& destination, double dx, double dy, const std::string& vehicle, 
          std::string status, std::string DFirst, std::string DLast, std::string DPhone){
        
          }

    // Getters
    std::string Rider::getFirst() const {
        return first;
    }
    std::string getLast() const {
        return last;
    }
    std::string getGender() const {
        return gender;
    }
    int getAge() const {
        return age;
    }
    std::string getPhone() const {
        return phone;
    }
    double getRating() const {
        return rating;
    }
    double getX() const {
        return x;
    }
    double getY() const {
        return y;
    }
    std::string getLocation() const {
        return location;
    }
    std::string getDestination() const {
        return destination;
    }
    double getDX() const {
        return dx;
    }
    double getDY() const {
        return dy;
    }
    std::string getVehicle() const {
        return vehicle;
    }
    std::string getStatus() const {
        return status;
    }
    std::string getDriverFirst() const {
        return DFirst;
    }
    std::string getDriverLast() const {
        return DLast;
    }
    std::string getDriverPhone() const {
        return DPhone;
    }

    // Setters
    void setRiderStatus(const std::string& newStatus) {
        status = newStatus;
    }
    void setDriver(const std::string& newDFirst, const std::string& newDLast, const std::string& newDPhone) {
        DFirst = newDFirst;
        DLast = newDLast;
        DPhone = newDPhone;
    }
    void setRiderPos(const double& newx, const double& newy) {
        x = newx;
        y = newy;
    }
    void setDriverPos(const double& newdx, const double& newdy) {
        dx = newdx;
        dy = newdy;
    }
    void setDriver(const std::string& newDFirst, const std::string& newDLast, const std::string& newDPhone) {
        DFirst = newDFirst;
        DLast = newDLast;
        DPhone = newDPhone;
    }