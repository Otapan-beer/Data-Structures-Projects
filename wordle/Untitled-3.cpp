#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

//convert string into vector of integers contianing the dimensions
std::vector<int> getDimensions(const std::string& line) {
    std::vector<int> dimensions;
    std::stringstream ss(line);
    int number;
    while (ss >> number) {
        dimensions.push_back(number);
    }
    return dimensions;
}

//check if we can place the word in the grid
bool canPlace(const std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, int width, int height) {
    int len = word.length();
    int endX = col + dx * (len - 1);
    int endY = row + dy * (len - 1);

    //placement within border check
    if (endX < 0 || endY < 0 || endX >= width || endY >= height) return false;

    //check if the word can be placed in the grid
    for (int i = 0; i < len; i++) { //iterate through the word
        if (grid[row][col] != '.' && grid[row][col] != word[i]) return false; //if the character is not '.' and not equal to the word[at index] character
        row += dy;      //change row
        col += dx;      //change column
    }
    return true;        //return true if the word can be placed
}

//place the word in the grid (after we check with canPlace)
std::vector<char> placeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy) {
    std::vector<char> originalChars;
    for (size_t i = 0; i < word.length(); i++) { //iterate through the word
        originalChars.push_back(grid[row][col]); //store the original character
        grid[row][col] = word[i];                //place the new character
        //go to next pos
        row += dy;
        col += dx;
    }
    return originalChars; //return the original characters just in case
}

//remove the word from the grid
void removeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, const std::vector<char>& originalChars) {
    for (size_t i = 0; i < word.size(); i++) {  //iterate through the word blah blah blah you get the drill by now
        grid[row][col] = originalChars[i];
        row += dy;
        col += dx;
    }
}

//check if the forbidden word is present in the grid 
bool isForbiddenWordPresent(const std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords) {
    int height = grid.size();
    int width = grid[0].size();

    for (const std::string& word : forbiddenWords) { //iterate through the forbidden words
        int len = word.length();

        for (int row = 0; row < height; ++row) { //iterate through the rows
            for (int col = 0; col < width; ++col) { //iterate through the columns
                // 8 possible directions
                std::vector<std::pair<int, int>> directions = {
                    {1, 0}, {0, 1}, {-1, 0}, {0, -1},  // Right, Down, Left, Up
                    {1, 1}, {-1, -1}, {1, -1}, {-1, 1} // Diagonal directions
                };

                for (std::pair<int, int> direction : directions) { //iterate through the directions
                    int dx = direction.first;
                    int dy = direction.second;
                    
                    std::string candidate;
                    int x = col, y = row;

                    for (int i = 0; i < len; ++i) { //iterate through the word
                        if (x < 0 || y < 0 || x >= width || y >= height) break; // Out of bounds
                        candidate += grid[y][x]; // Add the character to the candidate
                        x += dx;
                        y += dy;
                    }

                    if (candidate == word) {
                        return true; // Forbidden word found
                    }
                }
            }
        }
    }

    return false;
}



bool findOneSolution(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, const std::set<std::string>& forbiddenWords) {
    if (index == words.size()) { // Base case
        return !isForbiddenWordPresent(grid, forbiddenWords); // Check if the forbidden word is present
    }

    std::string word = words[index]; // Get the word
    for (int row = 0; row < height; row++) { // Iterate through the rows
        for (int col = 0; col < width; col++) { // Iterate through the columns
            std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}}; // 8 possible directions
            for (auto [dx, dy] : directions) { // Iterate through the directions
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::vector<char> originalChars = placeWord(grid, word, row, col, dx, dy);
                    if (findOneSolution(grid, words, index + 1, width, height, forbiddenWords)) {
                        return true;
                    }
                    removeWord(grid, word, row, col, dx, dy, originalChars);
                }
            }
        }
    }
    return false;
}

void findAllSolutions(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, std::set<std::vector<std::string>>& uniqueSolutions, const std::set<std::string>& forbiddenWords) {
    if (index == words.size()) {
        if (!isForbiddenWordPresent(grid, forbiddenWords)) {
            uniqueSolutions.insert(grid);
        }
        return;
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            for (auto [dx, dy] : directions) {
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::vector<char> originalChars = placeWord(grid, word, row, col, dx, dy);
                    findAllSolutions(grid, words, index + 1, width, height, uniqueSolutions, forbiddenWords);
                    removeWord(grid, word, row, col, dx, dy, originalChars);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <input.txt> <output.txt> <command>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }

    std::string line;
    std::vector<std::string> data;
    while (std::getline(file, line)) {
        data.push_back(line);
    }
    file.close();

    if (data.empty()) {
        std::cerr << "Input file is empty\n";
        return 1;
    }

    std::vector<int> dimensions = getDimensions(data[0]);
    if (dimensions.size() != 2) return 1;
    int width = dimensions[0], height = dimensions[1];

    data.erase(data.begin());
    std::vector<std::string> plus;
    std::set<std::string> forbiddenWords;

    for (const std::string& entry : data) {
        if (entry[0] == '+') {
            plus.push_back(entry.substr(2));
        } else if (entry[0] == '-') {
            forbiddenWords.insert(entry.substr(2));
        }
    }

    std::vector<std::string> grid(height, std::string(width, '.'));
    std::ofstream output(argv[2]);
    std::string command(argv[3]);

    if (command == "one_solution") {
        if (findOneSolution(grid, plus, 0, width, height, forbiddenWords)) {
            output << "1 solution(s)\nBoard:\n";
            for (const auto& row : grid) output << row << "\n";
        } else {
            output << "No solutions found\n";
        }
    } else if (command == "all_solutions") {
        std::set<std::vector<std::string>> uniqueSolutions;
        findAllSolutions(grid, plus, 0, width, height, uniqueSolutions, forbiddenWords);
        output << uniqueSolutions.size() << " solution(s)\n";
        for (const auto& sol : uniqueSolutions) {
            output << "Board:\n";
            for (const auto& row : sol) output << row << "\n";
        }
    } else {
        std::cerr << "Invalid keyword\n";
        return 1;
    }

    output.close();
    return 0;
}
