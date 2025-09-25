#include <iostream> // read/write
#include <fstream> // in/out files
#include <string> // strings
#include <vector> // vectors

class Driver {
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
    std::string vehicle;
    std::string status;
    std::string RFirst;
    std::string RLast;
    std::string RPhone;

public:
    // Constructor
    Driver(); //default
    Driver(const std::string& first, const std::string& last, const std::string& gender, 
           const int& age, const std::string& phone, const double& rating, double x, double y, 
           const std::string& vehicle, std::string status, 
           std::string RFirst, std::string RLast, std::string RPhone);

    // Getters
    std::string getFirstName();

    std::string getLastName();

    std::string getGender() const;

    int getAge() const;

    std::string getPhone();

    double getRating() const;

    double getX() const;

    double getY() const;

    std::string getVehicle() const;

    std::string getStatus() const;

    std::string getRiderFirstName() const;

    std::string getRiderLastName() const;

    std::string getRiderPhone() const;

    // Setters
    void setDriverStatus(const std::string& newStatus);

    void setRider(const std::string& newRFirst, const std::string& newRLast, const std::string& newRPhone);

    void setDriverPos(const double& newx, const double& newy);

    //Functions

};
