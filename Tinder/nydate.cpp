#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // include this so that we can use stringstream
#include <vector>
#include <list>
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include "date.h"

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

std::vector<std::string> splitstring(std::string line){ //I have tinkered with the provided code

    if(line.size() == 0){
        return std::vector<std::string>();
    }
    std::vector<std::string> phones;
    std::string longString = line;
    // create a stringstream to tokenize the long string
    std::istringstream iss(longString);
    std::string phoneNumber;

    // tokenize the long string using the underscore delimiter
    while (std::getline(iss, phoneNumber, '_')) {
            phones.push_back(phoneNumber);
    }
    return phones;
}

void splitline (std::string line, Node*& head, Node*& tail) { //splits the string into a vector and then assigns variables one by one before creating Node
    //split strings into vector
    std::stringstream ss(line);
    std::string variable;
    std::vector<std::string> storage;
    while(ss >> variable){
        storage.push_back(variable);
    }  

    //assign variables
    std::string name = storage[0];
    int age = std::stoi(storage[1]);
    std::string gender = storage[2];
    std::string phone = storage[3];
    std::string profession = storage [4];
    std::string school = storage[5];
    double latitude = std::stod(storage[6]);
    double longitude = std::stod(storage[7]);
    bool premium;
    if (storage[8] == "true"){
        premium = true;
    }
    else if (storage[8] == "false"){
        premium = false;
    }
    else{
        std::cout << "error bool\n";
        return;
    }
    int minimum = std::stoi(storage[9]);
    int maximum = std::stoi(storage[10]);
    double distance = std::stod(storage[11]);
    std::string preferred = storage[12];
    std::vector<std::string> users = splitstring(storage[13]);

    //create node
    if (head == nullptr){
        head = new Node(name, age, gender, phone, profession, school, latitude, longitude, 
            premium, minimum, maximum, distance, preferred, users);
        head->next = nullptr;
        tail = head;
        return;
    }
    else{
        tail->next = new Node(name, age, gender, phone, profession, school, latitude, longitude, 
            premium, minimum, maximum, distance, preferred, users);
        tail = tail->next;
        return;
    }
}

void printing(const std::string& name, int age, const std::string& profession,
    const std::string& school, std::ofstream& output) {
    output << name << " " << age << "\n";
    if (profession != "Undisclosed") {
    std::string text = profession;
    std::replace(text.begin(), text.end(), '_', ' ');
    output << text << "\n";
    }
    if (school != "Undisclosed") {
    std::string text = school;
    std::replace(text.begin(), text.end(), '_', ' ');
    output << text << "\n";
    }
}



int main(int argc, char* argv[]) {
    int printcheck = 0; //for spacing

    //Debugging Output (see input commands, will comment later)
    // std::cout << "Number of arguments: " << argc << std::endl;
    // for (int i = 0; i < argc; ++i) {
    //     std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    // }

    //Rest of main

    if (argc < 5) { //just to be safe
        std::cerr << "Usage: " << argv[0] << " <users.txt> <output.txt> <profile> <command> and maybe something extra\n";
        return 1;
    }
                                       // test case 22 input I have gotten from my Debugging Output
                                       //./a.out - we do nothing
    std::string inputFile = argv[1];   // users_medium1.txt
    std::string outputFile = argv[2];  // output.txt
    std::string phoneNumber = argv[3]; // 410-750-5502
    std::string keyword = argv[4];     // unmatch
    std::string extra;                 // created here so it is accesible from the whole main and not just the if (argc > 5)
    if (argc > 5) {                    // had to ask ChatGTP why is it not working (I had argv instead of argc)
        extra = argv[5];   // 318-125-5013
    }

    //setup pointers
    Node* head = nullptr;
    Node* tail = nullptr;
    Node* current = nullptr;

    //read the file and split it into seperate strings and create seperate nodes
    std::ifstream file(inputFile);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            splitline(line, head, tail); // splits the line and creates the Node for us
        }
    }
    file.close();

    std::ofstream output(outputFile, std::ios::app); //aparently this makes it so I can parse it to the printing function
    if (!output.is_open()) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return 1;
    }
//profile
    if (keyword == "profile"){
        current = head;
        Node* user = nullptr;

        //find the user
        while (current) {
            if (current->getphone() == phoneNumber) {
                user = current;
                break;
            }
            current = current->next;
        }

        if (!user) { //debugging
            output << "Error: User not found.\n";
            return 1;
        }
        int count = 0;
        //searching matches
        current = head;
        while (current) {
            //do not search the user (did it by phone number, it is easier)
            if (current->getphone() == phoneNumber) {
                current = current->next;
                continue;
            }


            //check age pref
            if (current->getage() < user->getminimum() || current->getage() > user->getmaximum()) {
                current = current->next;
                continue;
            }

            //distance
            double dist = calculateDistance(user->getlatitude(), user->getlongitude(),
                                            current->getlatitude(), current->getlongitude());

            //if the distance is too far, go next
            if (dist > user->getdistance()) {
                current = current->next;
                continue;
            }

            //if user didn't preffer both genders and the preffered is not matching, go next
            if (user->getpreferred() != "Both" && user->getpreferred() != current->getgender()) {
                current = current->next;
                continue;
            }

            //output
            if (printcheck > 0) {
                output << "\n";//seperate line
            }
            printing(current->getname(), current->getage(), current->getprofession(), current->getschool(), output);
            count++; //we have more than one match
            printcheck++;
            current = current->next; //go next
        }
        if (count == 0) { //gota probably stay virgin forever
            output << "There are no users matching with your preference at this moment.\n"; //why is it preferred, but also preference??? Where did the r go
        }
    }
//match
    else if (keyword == "match") { 
        printcheck = 0;
        current = head;
        Node* user = nullptr;

        // Find the user in the linked list
        while (current) {
            if (current->getphone() == phoneNumber) {
                user = current;
                break;
            }
            current = current->next;
        }

        if (!user) {
            output << "Error: User not found.\n";
            return 1;
        }

        // Get the vector with matches
        std::vector<std::string> matches = user->getusers();

        if (matches.empty()) { 
            output << "You do not have any matches at this moment.\n";
            return 0;
        }

        bool hasValidMatch = false;

        // Print mutual matches only
        for (const std::string& matchPhone : matches) {
            Node* matchNode = head;  // Start from the head each time
            while (matchNode) {
                if (matchNode->getphone() == matchPhone) { 
                    std::vector<std::string> otherUserMatches = matchNode->getusers();

                    if (std::find(otherUserMatches.begin(), otherUserMatches.end(), phoneNumber) != otherUserMatches.end()) {
                        if (printcheck > 0) {
                            output << "\n"; // Formatting
                        }
                        printing(matchNode->getname(), matchNode->getage(), matchNode->getprofession(), matchNode->getschool(), output);
                        printcheck++;
                        hasValidMatch = true;
                    }
                }
                matchNode = matchNode->next;  // Ensure this moves forward to prevent infinite loop
            }
        }
        if (!hasValidMatch) {
            output << "You do not have any matches at this moment.\n";
        }
    }


//like (fucking hell)
    else if (keyword == "like") {
        current = head; // Initialize before searching
        printcheck = 0;
        while (current != nullptr) {
            if (current->getphone() == phoneNumber) {
                if (current->getpremium() == false) { //need premium for this
                    output << "Only premium users can view who liked you.\n";
                } 
                else { //has premium
                    bool found = false;
                    Node* other = head;
                    while (other != nullptr) {
                        std::vector<std::string> likedUsers = other->getusers(); //get the vector of users this user liked
                        for (size_t i = 0; i < likedUsers.size(); i++) { //search the vector one by one
                            if (likedUsers[i] == phoneNumber) { //we found our user as liked
                                if(printcheck > 0) {
                                    output << "\n"; //spacing
                                }
                                printcheck++;
                                printing(other->getname(), other->getage(), other->getprofession(), other->getschool(), output);
                                found = true; //we have at least one find
                            }
                        }
                        other = other->next; //go next
                    }
                    if (found == false) { //you are really going to most likely die alone
                        output << "You have not received any likes so far.\n";
                    }
                }
                break;
            }
            current = current->next;
        }
    }
//unmatch
    else if (keyword == "unmatch") {
        printcheck = 0;
        if (extra.empty()) {
            output << "Error: No second phone number provided for unmatching.\n";
            return 1;
        }

        Node* user = nullptr;
        Node* otherUser = nullptr;

        // Find both users in the linked list
        current = head;
        while (current) {
            if (current->getphone() == phoneNumber) user = current;
            if (current->getphone() == extra) otherUser = current;
            if (user && otherUser) break; // Stop searching once both are found
            current = current->next;
        }

        if (!user || !otherUser) { // Prevent segmentation faults
            output << "Error: One or both users not found.\n";
            return 1;
        }

        // Check if both users have each other in their match lists
        std::vector<std::string> userMatches = user->getusers();
        std::vector<std::string> otherUserMatches = otherUser->getusers();

        bool mutualMatch = std::find(userMatches.begin(), userMatches.end(), extra) != userMatches.end() &&
                        std::find(otherUserMatches.begin(), otherUserMatches.end(), phoneNumber) != otherUserMatches.end();

        if (!mutualMatch) {
            output << "Error: Users are not mutually matched, so they cannot unmatch.\n";
            return 1;
        }

        // Remove each other from their match lists
        user->removeUser(extra);
        otherUser->removeUser(phoneNumber);

        // Print updated match lists
        output << user->getname() << "'s match list:\n\n";
        if (user->getusers().empty()) {
            output << "You do not have any matches at this moment.\n\n";
        } else {
            for (const std::string& matchPhone : user->getusers()) {
                Node* matchNode = head;
                while (matchNode) {
                    if (matchNode->getphone() == matchPhone) {
                        if(printcheck > 0){
                            output << "\n"; // Formatting
                        }
                        printcheck++;
                        printing(matchNode->getname(), matchNode->getage(), matchNode->getprofession(), matchNode->getschool(), output);
                        break;
                    }
                    matchNode = matchNode->next;
                }
            }
        }

        output << "======\n\n";
        // printcheck = 0;
        output << otherUser->getname() << "'s match list:\n\n";
        if (otherUser->getusers().empty()) {
            output << "You do not have any matches at this moment.\n";
        } else {
            for (const std::string& matchPhone : otherUser->getusers()) {
                Node* matchNode = head;
                while (matchNode) {
                    if (matchNode->getphone() == matchPhone) {
                        if(printcheck > 0){
                            output << "\n"; // Formatting
                        }
                        printcheck++;
                        printing(matchNode->getname(), matchNode->getage(), matchNode->getprofession(), matchNode->getschool(), output);
                        break;
                    }
                    matchNode = matchNode->next;
                }
            }
        }
    }
//block
    else if (keyword == "block") {
    printcheck = 0;

    if (extra.empty()) {
        output << "Error: No second phone number provided for blocking.\n";
        return 1;
    }

    Node* user = nullptr;
    Node* blockedUser = nullptr;

    // Find both users in the linked list
    current = head;
    while (current) {
        if (current->getphone() == phoneNumber) user = current;
        if (current->getphone() == extra) blockedUser = current;
        if (user && blockedUser) break;
        current = current->next;
    }

    if (!user || !blockedUser) {
        output << "Error: One or both users not found.\n";
        return 1;
    }

    // Remove each other from match lists
    user->removeUser(extra);
    blockedUser->removeUser(phoneNumber);

    // Print user's match list (excluding blocked user)
    output << "profiles shown to " << user->getname() << ":\n\n";
    current = head;
    printcheck = 0;
    bool userHasMatches = false;

    while (current) {
        if (current->getphone() == phoneNumber || current->getphone() == extra) {
            current = current->next;
            continue;
        }

        // Check if current user meets preferences
        if (current->getage() < user->getminimum() || current->getage() > user->getmaximum()) {
            current = current->next;
            continue;
        }

        double dist = calculateDistance(user->getlatitude(), user->getlongitude(),
                                        current->getlatitude(), current->getlongitude());

        if (dist > user->getdistance()) {
            current = current->next;
            continue;
        }

        if (user->getpreferred() != "Both" && user->getpreferred() != current->getgender()) {
            current = current->next;
            continue;
        }

        if (printcheck > 0) {
            output << "\n"; // Formatting
        }
        printing(current->getname(), current->getage(), current->getprofession(), current->getschool(), output);
        printcheck++;
        userHasMatches = true;
        current = current->next;
    }

    if (!userHasMatches) {
        output << "There are no users matching with your preference at this moment.\n";
    }


    output << "\n======\n\n";

    // Print blocked user's match list (excluding the blocking user)
    output << "profiles shown to " << blockedUser->getname() << ":\n\n";
    current = head;
    printcheck = 0;
    bool blockedUserHasMatches = false;

    while (current) {
        if (current->getphone() == phoneNumber || current->getphone() == extra) {
            current = current->next;
            continue;
        }

        // Check if current user meets blocked user's preferences
        if (current->getage() < blockedUser->getminimum() || current->getage() > blockedUser->getmaximum()) {
            current = current->next;
            continue;
        }

        double dist = calculateDistance(blockedUser->getlatitude(), blockedUser->getlongitude(),
                                        current->getlatitude(), current->getlongitude());

        if (dist > blockedUser->getdistance()) {
            current = current->next;
            continue;
        }

        if (blockedUser->getpreferred() != "Both" && blockedUser->getpreferred() != current->getgender()) {
            current = current->next;
            continue;
        }

        if (printcheck > 0) {
            output << "\n"; // Formatting
        }
        printing(current->getname(), current->getage(), current->getprofession(), current->getschool(), output);
        printcheck++;
        blockedUserHasMatches = true;
        current = current->next;
    }

    if (!blockedUserHasMatches) {
        output << "There are no users matching with your preference at this moment.\n";
    }
}

//check    
    else {
        std::cout << "command issue\n";
        return 1;
    }
    output.close();
    // Clean up memory
    Node* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }
}