#include "date.h"
#include <sstream>
#include <algorithm>
//constructor
Node::Node(std::string Name, int Age, std::string Gender, std::string Phone, std::string Profession, 
    std::string School, double Latitude, double Longitude, bool Premium, int Minimum, int Maximum, 
    double Distance, std::string Preferred, std::vector<std::string> Users) : 
    next(nullptr), name(Name), age(Age), gender(Gender), phone(Phone), profession(Profession), school(School), 
    latitude(Latitude), longitude(Longitude), premium(Premium), minimum(Minimum), maximum(Maximum), 
    distance(Distance), preferred(Preferred), users(Users) {}

//getters
std::string Node::getname() const {
    return name;
}
int Node::getage() const {
    return age;
}
std::string Node::getgender() const {
    return gender;
}
std::string Node::getphone() const {
    return phone;
}
std::string Node::getprofession() const {
    return profession;
}
std::string Node::getschool() const {
    return school;
}
double Node::getlatitude() const {
    return latitude;
}
double Node::getlongitude() const {
    return longitude;
}
bool Node::getpremium() const {
    return premium;
}
int Node::getminimum() const {
    return minimum;
}
int Node::getmaximum() const {
    return maximum;
}
double Node::getdistance() const {
    return distance;
}
std::string Node::getpreferred() const {
    return preferred;
}
std::vector<std::string>& Node::getusers() { //pass by refference because we will be removing numbers
    return users;
}

//functions
void Node::removeUser(const std::string& phoneNumber) { //search from the beginning of the vector
    std::vector<std::string>::iterator it = std::find(users.begin(), users.end(), phoneNumber); //iterator enables me to go throug containers
    if (it != users.end()) { //check if we found the number
        users.erase(it); //we found it, removing it, erase should move the elements after the number one left
    }
}
