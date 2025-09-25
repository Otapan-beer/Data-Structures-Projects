#include <iostream> // read/write
#include <fstream> // in/out files
#include <string> // strings
#include <vector> // vectors

class Rider {
private:
    // Variables
    std::string first;
    std::string last;
    std::string gender;
    int age;
    std::string phone;
    double rating;
    double x;
    double y;
    std::string location;
    std::string destination;
    double dx;
    double dy;
    std::string vehicle;
    std::string status;
    std::string DFirst;
    std::string DLast;
    std::string DPhone;

public:
    //Constructor
    Rider(); //default
    Rider(const std::string& cfirst, const std::string& clast, const std::string& cgender, int cage, 
        const std::string& cphone, double crating, const std::string& clocation, double cx, double cy, 
        const std::string& cdestination, double cdx, double cdy, const std::string& cvehicle, 
        std::string cstatus, std::string cDFirst, std::string cDLast, std::string cDPhone);

    //Getters
    std::string getFirst() const;

    std::string getLast() const;

    std::string getGender() const;

    int getAge() const;

    std::string getPhone() const;

    double getRating() const;

    double getX() const;

    double getY() const;

    std::string getLocation() const;

    std::string getDestination() const;

    double getDX() const;

    double getDY() const;

    std::string getVehicle() const;

    std::string getStatus() const;

    std::string getDriverFirst() const;

    std::string getDriverLast() const;

    std::string getDriverPhone() const;

    void setRiderStatus(const std::string& newStatus);

    void setRiderPos(const double& newx, const double& newy);

    void setDriverPos(const double& newdx, const double& newdy);

    void setDriver( std::string newDFirst,  std::string newDLast,  std::string newDPhone);
};
