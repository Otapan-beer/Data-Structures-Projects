// Otakar Lichnovsky, started on 12th of January 2025

//Hours date (plus) - total:
// 1/12/2025 (+0.5) - 0.5
// 1/14/2025 (+2.5) - 3     ... I do count the time spent in office hours resolving the issues and (slowly) creating the code
// 1/15/2025 (+8.5) - 11.5  ... I added the hours the next day, I was too tired to do it yesterday immediately.
// 1/16/2025 (+7.0) - 18.5  ... I am counting 3AM as yesterday FYI

// Logs (in detail description of time spent, COPY OF README): 
// 1/12/2025 I started this programm on sunday, but nothing ran correctly. I will have to go to office hours again to fix it

// 1/14/2025 I had to do some homework on monday but a friend solved my issues, turns I was in the wrong directory and 
// I have spent like 2-3 hours looking up all the guides on google that I could lol. I am still learning from guides though.

// 1/15/2025 I am really sweating today, I have spend a couple more hours looking up tutorials on C++ to learn the syntax, but
// it didn't help much. I have woken up around 10 am and I am coding since about 11am. I went to the office hours yesterday
// and I got almost 25 lines of code with the first read function. I do not know how am I supposed to treat the "next"
// and "previous " actions in the output file. So far, I have spent over 4.5 hours writing 30 lines of code not accounting for
// the time spent on tutorials just today, over 6 with tutorials.

// 1/16/2025 Yesterday I returned from labs and worked on the code for about 4 more hours before going to sleep
// Also, I made about 50 lines of code in that time, big progress
// I am just finishing a final problem with the move function not working correctly by the index of 1 and I will be done.
// Today I have added in total extra 7 hours to finish the code off.


//All of the #include stuff
#include <iostream> //read/write
#include <fstream> //in/out files
#include <filesystem> //file creating etc. in case I need it
#include <string> //strings
#include <vector> //vectors
#include <algorithm> // For std::swap



int main(int argc, char* argv[]){ //I know I am not supposed to put everything here, but I have no idea how to do so without using
// classes that we didnt go over yet as of 1/12/2025, soo...

    std::string play = argv[1]; //playlist file name is the first argument
    std::string act = argv[2]; //action file name is the second argument
    std::string out = argv[3]; //output file name is the third argument

//defining some basic variables that are going to be used
    std::string line; //creating line string
    std::string current = " current"; //the "current song" indicator
    int index = -1; //to keep the track of the index of the current song (-1 to make sure we dont know it yet)
    std::vector <std::string> playlist; //create an empty vector where the playlist will be stored

//opening the files
    std::ifstream listfile(play);
    std::ifstream actions(act);
    std::ofstream output(out);

//setting up the loop to get the list saed into the vector
    while (std::getline(listfile, line)){
        playlist.push_back(line);
    }

//setting up the main code part with the actions
    while (std::getline(actions, line)){
        size_t length = line.length(); //find the length of the string in total
        size_t firstpos = line.find(" "); //find the first space
        if (firstpos != std::string::npos){ //assuring that this will happen only if space exists
            std::string command = line.substr(0, firstpos); // get the command isolated
            std::string next = line.substr(firstpos + 1, length); // remove the command from the string
            length -= firstpos;

            if (command == "add"){ //if the command is add
                playlist.push_back(next); //just pushback the song into last spot
            }
            else if (command == "remove"){
                for (size_t i = 0; i < playlist.size(); ++i) { 
                    if (playlist[i].find(next) != std::string::npos) {
                        if (index > i){ //if the removed song is above the current song, the song will move one up
                            index--;
                        }
                        playlist.erase(playlist.begin() + i);
                        break;
                    }
                }
            }

            else if (command == "move"){
                size_t len = next.length();
                size_t lastpos = next.rfind(" ");
                std::string where = next.substr(lastpos + 1, len);
                std::string song = next.substr(0, lastpos);
                int number = std::stoi(where); //change the string captured to the int - had to use "other sources"
                number --;
                for (size_t i = 0; i < playlist.size(); ++i) { 
                    if (playlist[i].find(song) != std::string::npos) {
                        std::string placeholder;
                        if (i < number){ // if we are moving the song back
                            placeholder = playlist[i];
                            for (int n = i; n < number; n++){
                                playlist[n] = playlist[n+1];
                            }
                            playlist[number] = placeholder;
                            for (size_t i = 0; i < playlist.size(); i++) { //set up the loop to find the index of current
                                if (playlist[i].find(current) != std::string::npos){
                                    index = i; //update the index
                                } //if found, records the index and then breaks the loop
                            }
                        }
                        else if (i > number){ // if we are moving the song forward
                            placeholder = playlist[i];
                            for (int n = i-1; n >= number; n--){
                                playlist[n+1] = playlist[n];
                            }
                            playlist[number] = placeholder;
                        }
                            for (size_t i = 0; i < playlist.size(); i++) { //set up the loop to find the index of current
                                if (playlist[i].find(current) != std::string::npos){
                                    index = i; //update the index
                                } //if found, records the index and then breaks the loop
                            }
                        break;
                    }
                }
            }
        } //everyting after is assuming there is no space in the command
    
        else if (line == "next"){ //put the current on the next track
            size_t size = playlist.size();
            std::string hold;
            std::string check = " current"; // I have no realized I have created this twice
            if (index != -1) { //the index was already found
                hold = playlist[index]; //just to hold the original version of the string
                size_t pos = hold.find(check); // The method ain't too effective, but it finds the position of " current"
                if (pos != std::string::npos) { // another safecheck
                    hold.erase(pos, check.length()); // delete the "current" from this string
                    playlist[index] = hold; // keep the string just without the " current"
                }
                index ++; // now we move to the next song in the playlist
                if ((index) >= size){ //assuming this was already the last song, we move to the first song
                    index = 0; //set the index to 0
                    hold = playlist[0]; //get the original string
                    hold.append(check); //add " current" to the end
                    playlist[0] = hold; //set the hold as vector[0]
                }
                else{ //if that was not the last song
                    hold = playlist[index]; // get the next song
                    hold.append(check); // add " current" to it
                    playlist[index] = hold; // set it with the indicator
                }
            } 
            else { // the index was not found
                for (size_t i = 0; i < playlist.size(); i++) { //set up the loop to find the index of current
                    if (playlist[i].find(current) != std::string::npos){ //if found, records the index and then breaks the loop
                        hold = playlist[i]; //just to hold the original version of the string
                        size_t pos = hold.find(check); // I know it's not verry effective, but it serves the purpose
                        if (pos != std::string::npos) { // another safecheck
                            hold.erase(pos, check.length()); // delete the "current" from this string
                            playlist[i] = hold;
                        }
                        index = i+1;
                        if ((index) >= size){ //assuming this was already the last song, we move to the first song
                            index = 0; //set the index to 0
                            hold = playlist[0]; //get the original string
                            hold.append(check); //add " current" to the end
                            playlist[0] = hold; //set the hold as vector[0]
                        }
                        else{
                            hold = playlist[index]; // get the next song
                            hold.append(check); // add " current" to it
                            playlist[index] = hold; // set it with the indicator
                        }
                        break;
                    }
                }
            }

        }

        else if (line == "previous"){ //put the current on the previous track
            size_t size = playlist.size();
            std::string hold;
            std::string check = " current";
            if (index != -1) { //the index was already found
                hold = playlist[index]; //just to hold the original version of the string
                size_t pos = hold.find(check); // The method ain't too effective, but it finds the position of " current"
                if (pos != std::string::npos) { // another safecheck
                    hold.erase(pos, check.length()); // delete the "current" from this string
                    playlist[index] = hold; // keep the string just without the " current"
                }
                index --;
                if ((index) < 0){ //assuming this was the first song, we move to the last song
                    index = size - 1; //set the index to the last song
                    hold = playlist[index]; //get the original string
                    hold.append(check); //add " current" to the end
                    playlist[index] = hold; //set the hold as vector[last song]
                }
                else{ //if that was not the last song
                    hold = playlist[index]; // get the next song
                    hold.append(check); // add " current" to it
                    playlist[index] = hold; // set it with the indicator
                }
            }
            else{ //index was not found
                for (size_t i = 0; i < playlist.size(); i++) { //set up the loop to find the index of current
                    if (playlist[i].find(current) != std::string::npos){ //if found, records the index and then breaks the loop
                        hold = playlist[i]; //just to hold the original version of the string
                        size_t pos = hold.find(check); // I know it's not verry effective, but it serves the purpose
                        if (pos != std::string::npos) { // another safecheck
                            hold.erase(pos, check.length()); // delete the "current" from this string
                            playlist[i] = hold;
                        }
                        index = i-1;
                        if (index < 0){ //assuming this was already the first song, we move to the last song
                            index = size - 1; //set the index to end of the vector
                            hold = playlist[index]; //get the original string
                            hold.append(check); //add " current" to the end
                            playlist[index] = hold; //set the hold as vector[current song]
                        }
                        else{
                            hold = playlist[index]; // get the previous song
                            hold.append(check); // add " current" to it
                            playlist[index] = hold; // set it with the indicator
                        }
                        break;
                    }
                }
            }
        }
        else { //it should not come to this, but just in case, so the whole thing doesn't get bricked
            continue;
        }
    }
    for (const std::string song : playlist) {
        output << song << std::endl;
    }
    output.close();
}

