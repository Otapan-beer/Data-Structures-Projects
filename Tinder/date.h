#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // include this so that we can use stringstream
#include <vector>
#include <list>
#include <cmath>

class Node {
public:
    //constructor
    Node(std::string Name, int Age, std::string Gender, std::string Phone, std::string Profession, 
    std::string School, double Latitude, double Longitude, bool Premium, int Minimum, int Maximum, 
    double Distance, std::string Preferred, std::vector<std::string> Users);

    //public because it was said it is ok
    Node* next;
    std::string name;
    int age;
    std::string gender;
    std::string phone;
    std::string profession;
    std::string school;
    double latitude;
    double longitude;
    bool premium;
    int minimum;
    int maximum;
    double distance;
    std::string preferred;
    std::vector<std::string> users;

    //getters
    std::string getname() const;
    int getage() const;
    std::string getgender() const;
    std::string getphone() const;
    std::string getprofession() const;
    std::string getschool() const;
    double getlatitude() const;
    double getlongitude() const;
    bool getpremium() const;
    int getminimum() const;
    int getmaximum() const;
    double getdistance() const;
    std::string getpreferred() const;
    std::vector<std::string>& getusers(); //pass by refference because we will be removing numbers

    //functions
    void removeUser(const std::string& phoneNumber);
};

#endif
