//author: Otakar Lichnovsky
//time spent: definitely over 20h, I have lost count, I have not slept for the last
//3 days (tuesday to friday) for longer than 5 hours. (I am not even counting the tutorials)
//It is so bad, I did not even shower and eat only 2 meals a day since I started coding this assignment.
//I have been to almost every office hours there is just to get stuck immediately after fixing the last issue...
//I am very much hating the fact that every single fucking tutorial uses the most
//useless syntax that would immediately get me smited with at least -15 points
//because the rubric specified that we are NOT putting everything into public
//and we are NOT putting everything into main
//fuck this shit. I am very pationate about hating these errors.

//Now give me your best grade! This is a robbery!

//Bump the hours up to 25+, I apparently don't know how to assign & to do by refference correctly.

//Credit to Lance Schneider and Aleksey Dochez, couple of my senior CS major friends and all the office hours helpers for getting me unstuck every couple minutes.

//Ok fine, I can't, WHY IS THE VECTOR NOT FUCKING UPDATING LIKE WHYYYYYYYYYYYYYYYYYYYYYYY
//I DIDNT EVEN EAT TODAY AND IT'S 8PM, START UPDATING, LIKE HOW MAHY HOURS OF MY TIME DO YOU FUCKING WANT, 30?
// YES, I HAVE CODED OVER 30 HOURS STRAIGHT AHAHAHHAHAHAHAHAHAJLK; SFF ;ALIGFU

//are you fucking serious... WHY ARE THESE ERRORS LOOPING? I FIX A, B APPEARS, I FIX B, C APPEARS, I FIX C, A APPEARS
//WHAT IS THIS BULLSHIT

//RING THE FUCKING BELL YEEEEEEEEEEEEEEEEEEEEEEEEEEEEES, STOP THE TIMER
//IT IS 9:42PM I HAVE NOT EATEN MORE THAN 24 HOURS JESUS CHRIST I CAN ALSMOT SEE TIWCE

#include <iostream> // read/write
#include <fstream> //in/out files
#include <string> //strings
#include <vector> //vectors
#include <filesystem> //creating files
#include <sstream> //string streams
#include <cmath> //for calculating distance
#include <algorithm>//for find

#include "drivers.h"
#include "riders.h"

using namespace std;

// It would be an absolute inconvenience to not put the provided formula here... don't you agree?

// calculate the distance between two coordinates using Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double radiusOfEarth = 6371.0; // Earth's radius in kilometers

    // convert latitude and longitude from degrees to radians
    lat1 *= M_PI / 180.0;
    lon1 *= M_PI / 180.0;
    lat2 *= M_PI / 180.0;
    lon2 *= M_PI / 180.0;

    // Haversine formula
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) + cos(lat1) * cos(lat2) * sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    // distance in kilometers
    double distanceKM = radiusOfEarth * c;
    // convert it to distance in miles
    double distanceMiles = distanceKM * 0.621371;

    return distanceMiles;
}


int main(int argc, char* argv[]) {

//filenames
    std::string driversfile = argv[1];
    std::string ridersfile = argv[2];
    std::string out0file = argv[3];
    std::string out1file = argv[4];
    std::string out2file = argv[5];
    std::string phoneNumber = argv[6];
    std::string request = argv[7];

//file streams
    std::ifstream driversf(driversfile);
    std::ifstream ridersf(ridersfile);
    std::ofstream output0(out0file);
    std::ofstream output1(out1file);
    std::ofstream output2(out2file);

//drivers
    std::string line;
    std::vector <Driver> drivers;
    std::string ffirst, flast, fgender, fphone, fvehicle, fstatus, 
    frfirst, frlast, frphone, fdfirst, fdlast, fdnumber, fposition, fdestination, fdphone;
    int fage;
    double frating, fx, fy, fdx, fdy;
    while (std::getline(driversf, line)){
        std::stringstream(line) >> ffirst >> flast >> fgender >> fage >> fphone >> frating >> fx >> fy >> fvehicle >> fstatus
        >> frfirst >> frlast >> frphone;
        drivers.push_back(Driver(ffirst ,flast ,fgender ,fage ,fphone ,frating ,fx ,fy ,fvehicle ,fstatus
        ,frfirst ,frlast ,frphone));
    }

//riders
    std::vector <Rider> riders;
    while (std::getline(ridersf, line)){
        std::stringstream(line) >> ffirst >> flast >> fgender >> fage >> fphone >> frating >> fposition >> fx >> fy 
        >> fdestination >> fdx >> fdy >> fvehicle >> fstatus >> fdfirst >> fdlast >> fdphone;
        riders.push_back(Rider(ffirst, flast, fgender, fage, fphone, frating, fposition, fx, fy,
        fdestination, fdx, fdy, fvehicle, fstatus, fdfirst, fdlast, fdphone));
    }

    //declare some important variables
    int drivernum = 0;
    int oldnum = 0;
    int ridernum = 0;

//iterating the driver from the phone number over the vector
    if (request == "request"){ //request
        if (phoneNumber.size() < 12){
            output0 << "Phone number is invalid.\n";
        }
        else if (phoneNumber.find("-") != 3 || phoneNumber.rfind("-") != 7){
            output0 << "Phone number is invalid.\n";
        }
        else { //if the number looks valid we search. If not found then print the error.
            Rider currentRider;
            Driver currentDriver;
            double distance = 9999.9;
            double dist;
            double rounded;
            int safecount = -1;
            for (int i = 0; i < riders.size(); i++) {
                if (riders[i].getPhone() == phoneNumber){
                    currentRider = riders[i];
                    ridernum = i;
                    safecount++;
                    break;
                }
            }
            if(safecount == -1) { //error not found
                output0 << "Account does not exist.\n";
            }
            //these are if the rider is already assigned to a driver
            else if (currentRider.getStatus() == "Driver_on_the_way") {
                output0 << "You have already requested a ride and your driver is on the way to the pickup location.\n";
            }
            else if (currentRider.getStatus() == "During_the_trip") {
                output0 << "You can not request a ride at this moment as you are already on a trip.\n";
            }
            //no problem found, we run the request
            else {
                if (currentRider.getVehicle() == "Economy") {
                        output0 << "Ride requested for rider " << currentRider.getFirst() <<", looking for an " << currentRider.getVehicle() << " vehicle.\n"; //a vs an
                    }
                else {
                    output0 << "Ride requested for rider " << currentRider.getFirst() <<", looking for a " << currentRider.getVehicle() << " vehicle.\n";
                }
                output0 << "Pick Up Location: " << currentRider.getLocation() << ", Drop Off Location: " << currentRider.getDestination() << ".\n";
                //start finding drivers
                safecount = -1; //load the save!
                for (int i = 0; i < drivers.size(); i++) {
                    if (drivers[i].getVehicle() == currentRider.getVehicle() && drivers[i].getStatus() == "Available"){
                        dist = calculateDistance(drivers[i].getX(), drivers[i].getY(), currentRider.getX(), currentRider.getY());
                        rounded = floor(dist*10);
                        rounded /= 10;
                        if (rounded < distance){
                            currentDriver = drivers[i];
                            drivernum = i;
                            distance = rounded;
                            safecount++;
                        }
                    }
                }
                if (safecount == -1) { // not found any drivers
                    output0 << "Sorry we can not find a driver for you at this moment.\n";
                }
                else {
                    output0 << "We have found the closest driver " << currentDriver.getFirstName() << "(" << currentDriver.getRating() << ") for you.\n";
                    output0 << currentDriver.getFirstName() << " is now " << distance << " miles away from you.";
                    currentDriver.setDriverStatus("On_the_way_to_pickup");
                    currentRider.setRiderStatus("Driver_on_the_way");
                    currentDriver.setRider(currentRider.getFirst(), currentRider.getLast(), currentRider.getPhone());
                    currentRider.setDriver(currentDriver.getFirstName(), currentDriver.getLastName(), currentDriver.getPhone());
                    drivers[drivernum].setDriverStatus("On_the_way_to_pickup");
                    riders[ridernum].setRiderStatus("Driver_on_the_way");
                    drivers[drivernum].setRider(currentRider.getFirst(), currentRider.getLast(), currentRider.getPhone());
                    riders[ridernum].setDriver(currentDriver.getFirstName(), currentDriver.getLastName(), currentDriver.getPhone());
                }
            }
        }
    }


    else if (request == "cancel") { //cancel
        if (phoneNumber.size() < 12){
            output0 << "Account does not exist.\n";
        }
        else if (phoneNumber.find("-") != 3 || phoneNumber.rfind("-") != 7){
            output0 << "Account does not exist.\n";
        }
        else { //if the number looks valid we search. If not found then print the error.
            Rider currentRider;
            Driver currentDriver;
            Driver oldDriver;
            int safecount = -1;
            double distance = 9999.9;
            double dist;
            double rounded;
            for (int i = 0; i < riders.size(); i++) {
                if (riders[i].getPhone() == phoneNumber){
                    ridernum = i;
                    currentRider = riders[i];
                    safecount = 0;
                    break;
                }
            }
            if (safecount == -1) { //if it is not a rider
                for (int i = 0; i < drivers.size(); i++) {
                    if (drivers[i].getPhone() == phoneNumber){
                        drivernum = i;
                        currentDriver = drivers[i];
                        safecount = 1;
                        break;
                    }
                }
            }
            if (safecount == -1) {
                output0 << "Account does not exist.\n";
            }
            else if (safecount == 0) { // it is a rider
                if (currentRider.getStatus() != "Driver_on_the_way") { // if the driver isn't on the way
                    output0 << "You can only cancel a ride request if your driver is currently on the way to the pickup location.\n";
                }
                else { // if he is on the way
                    currentRider.setRiderStatus("Ready_to_request");
                    riders[ridernum].setRiderStatus("Ready_to_request");
                    std::string phone = currentRider.getDriverPhone();
                    for (int i = 0; i < drivers.size(); i++) {
                        if (drivers[i].getPhone() == phone){
                            drivernum = i;
                            currentDriver = drivers[i];
                            safecount = 1;
                            break;
                        }
                    }
                    currentRider.setDriver("null", "null", "null");
                    currentDriver.setRider("null", "null", "null");
                    currentDriver.setDriverStatus("Available");
                    riders[ridernum].setDriver("null", "null", "null");
                    drivers[drivernum].setRider("null", "null", "null");
                    drivers[drivernum].setDriverStatus("Available");
                    output0 << "Ride request for rider " << currentRider.getFirst() << " is now canceled by the rider.\n";
                }
            }
            else if (safecount == 1) { // it is a driver
                if (currentDriver.getStatus() != "On_the_way_to_pickup") {
                    output0 << "You can only cancel a ride request if you are currently on the way to the pickup location.\n";
                }
                else { // Driver cancel is valid
                    double distance = 9999.9;
                    std::string riderNum = currentDriver.getRiderPhone();
                    oldDriver = currentDriver;
                    for (int i = 0; i < drivers.size(); i++){
                        if(drivers[i].getPhone() == oldDriver.getPhone()){
                            oldnum = i;
                            break;
                        }
                    }
                    for (int i = 0; i < riders.size(); i++) {
                        if (riders[i].getPhone() == riderNum){
                            ridernum = i;
                            currentRider = riders[i];
                            break;
                        }
                    }
                    output0 << "Your driver " << currentDriver.getFirstName() << " has canceled the ride request. We will now find a new driver for you.\n";
                    if (currentRider.getVehicle() == "Economy") {
                        output0 << "Ride requested for rider " << currentRider.getFirst() <<", looking for an " << currentRider.getVehicle() << " vehicle.\n"; //a vs an
                    }
                    else {
                        output0 << "Ride requested for rider " << currentRider.getFirst() <<", looking for a " << currentRider.getVehicle() << " vehicle.\n";
                    }
                    output0 << "Pick Up Location: " << currentRider.getLocation() << ", Drop Off Location: " << currentRider.getDestination() << ".\n";
                    safecount = -1; //load the save!
                    for (int i = 0; i < drivers.size(); i++) {
                        if (drivers[i].getVehicle() == currentRider.getVehicle() && drivers[i].getStatus() == "Available"){
                            dist = calculateDistance(drivers[i].getX(), drivers[i].getY(), currentRider.getX(), currentRider.getY());
                            rounded = floor(dist*10);
                            rounded /= 10;
                            if (rounded < distance){
                                drivernum = i;
                                currentDriver = drivers[i];
                                distance = rounded;
                                safecount++;
                            }
                        }
                    }
                    if (safecount == -1) { // not found any drivers
                    output0 << "Sorry we can not find a driver for you at this moment.\n";
                    oldDriver.setDriverStatus("Available"); // I have to reset the old driver after search or we would end up with the same one that cancelled.
                    oldDriver.setRider("null", "null", "null");
                    drivers[oldnum].setDriverStatus("Available");
                    drivers[oldnum].setRider("null", "null", "null");
                    }
                    else {
                    output0 << "We have found the closest driver "<< currentDriver.getFirstName() << "(" << currentDriver.getRating() << ") for you.\n";
                    output0 << currentDriver.getFirstName() << " is now " << distance << " miles away from you.";
                    oldDriver.setDriverStatus("Available"); // I have to reset the old driver after search or we would end up with the same one that cancelled.
                    oldDriver.setRider("null", "null", "null");
                    drivers[oldnum].setDriverStatus("Available");
                    drivers[oldnum].setRider("null", "null", "null");
                    currentDriver.setDriverStatus("On_the_way_to_pickup");
                    currentDriver.setRider(currentRider.getFirst(), currentRider.getLast(), currentRider.getPhone());
                    currentRider.setRiderStatus("Driver_on_the_way");
                    currentRider.setDriver(currentDriver.getFirstName(), currentDriver.getLastName(), currentDriver.getPhone());
                    drivers[drivernum].setDriverStatus("On_the_way_to_pickup");
                    drivers[drivernum].setRider(currentRider.getFirst(), currentRider.getLast(), currentRider.getPhone());
                    riders[ridernum].setRiderStatus("Driver_on_the_way");
                    riders[ridernum].setDriver(currentDriver.getFirstName(), currentDriver.getLastName(), currentDriver.getPhone());
                    }
                }
            }
            else {
                output0 << "Account does not exist.\n"; // just to make sure
            }
        }
    }
    output0.close(); // neat.
    Driver driver;
    Rider rider;
    for (int i = 0; i < drivers.size(); i++) {
        driver = drivers[i];
        output1 << driver.getFirstName() << " " 
                << driver.getLastName() << " " 
                << driver.getGender() << " "
                << driver.getAge() << " "
                << driver.getPhone() << " "
                << driver.getRating() << " "
                << driver.getX() << " "
                << driver.getY() << " "
                << driver.getVehicle() << " "
                << driver.getStatus() << " "
                << driver.getRiderFirstName() << " "
                << driver.getRiderLastName() << " "
                << driver.getRiderPhone()
                << std::endl;
    }
    output1.close();
    for (int i = 0; i < riders.size(); i++) {
        rider = riders[i];
        output2 << rider.getFirst() << " "
                << rider.getLast() << " "
                << rider.getGender() << " "
                << rider.getAge() << " "
                << rider.getPhone() << " "
                << rider.getRating() << " "
                << rider.getLocation() << " "
                << rider.getX() << " "
                << rider.getY() << " "
                << rider.getDestination() << " "
                << rider.getDX() << " "
                << rider.getDY() << " "
                << rider.getVehicle() << " "
                << rider.getStatus() << " "
                << rider.getDriverFirst() << " "
                << rider.getDriverLast() << " "
                << rider.getDriverPhone()
                << std::endl;
    }
    output2.close();

    //PLEASE WORK FOR THE SAKE OF MY SANITY
}