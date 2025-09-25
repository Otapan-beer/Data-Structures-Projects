#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

std::vector<int> getDimensions(const std::string& line) {
    std::vector<int> dimensions;
    if (line.empty()) {
        std::cerr << "dimension input err\n";
        return {};
    }

    std::stringstream ss(line);
    int number;
    while (ss >> number) {
        dimensions.push_back(number);
    }

    if (dimensions.size() != 2) {
        std::cerr << "dimension incorrect size " << dimensions.size() << " \n";
        return {};
    }

    return dimensions;
}

bool canPlace(const std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, int width, int height) {
    // check the word placement is within the grid and if the letters match the word (indexed)
    int len = word.length();
    int endX = col + dx * (len - 1);
    int endY = row + dy * (len - 1);

    if (endX < 0 || endY < 0 || endX >= width || endY >= height) {
        return false;
    }

    for (int i = 0; i < len; i++) {
        if (grid[row][col] != '.' && grid[row][col] != word[i]) {
            return false;
        }
        row += dy;
        col += dx;
    }
    return true;
}

std::vector<char> placeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy) {
    std::vector<char> originalChars;
    for (char ch : word) {
        originalChars.push_back(grid[row][col]);  // Store the original character
        grid[row][col] = ch;                      // Place the new character
        row += dy;
        col += dx;
    }
    return originalChars;
}


void removeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, const std::vector<char>& originalChars) {
    for (size_t i = 0; i < word.size(); i++) {
        grid[row][col] = originalChars[i];  // Restore original character
        row += dy;
        col += dx;
    }
}



bool onesol(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height) {
    if (index == words.size()) {
        return true;
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {{1, 0}, {1, -1}, {0, -1}, {0, 1}, {1, 1}, {-1, 0}, {-1, -1}, {-1, 1}};
            for (std::vector<std::pair<int, int>>::const_iterator it = directions.begin(); it != directions.end(); ++it) {
                int dx = it->first, dy = it->second;
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::vector<char> originalChars = placeWord(grid, word, row, col, dx, dy);
                    if (onesol(grid, words, index + 1, width, height)) {
                        return true;
                    }
                    removeWord(grid, word, row, col, dx, dy, originalChars);
                }                
            }
        }
    }
    return false;
}

void allsol(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, std::set<std::vector<std::string>>& uniqueSolutions) {
    if (index == words.size()) {
        uniqueSolutions.insert(grid);
        return;
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {{1, 0}, {1, -1}, {0, -1}, {0, 1}, {1, 1}, {-1, 0}, {-1, -1}, {-1, 1}};
            for (std::vector<std::pair<int, int>>::const_iterator it = directions.begin(); it != directions.end(); ++it) {
                int dx = it->first, dy = it->second;
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::vector<char> originalChars = placeWord(grid, word, row, col, dx, dy);
                    allsol(grid, words, index + 1, width, height, uniqueSolutions);
                    removeWord(grid, word, row, col, dx, dy, originalChars);
                }                
            }
        }
    }
}


// Function to determine a safe replacement character for empty spots
char findSafeFiller(const std::vector<std::string>& minus) {
    std::set<char> forbiddenLetters;
    for (std::vector<std::string>::const_iterator it = minus.begin(); it != minus.end(); ++it) {
        forbiddenLetters.insert(it->begin(), it->end());
    }

    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        if (forbiddenLetters.find(ch) == forbiddenLetters.end()) {
            return ch;
        }
    }
    return 'X'; // Default fallback
}

void replaceDots(std::vector<std::string>& grid, char filler) {
    for (std::vector<std::string>::iterator it = grid.begin(); it != grid.end(); ++it) {
        std::replace(it->begin(), it->end(), '.', filler);
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
        std::cerr << "Input file is empty or incorrect format\n";
        return 1;
    }

    std::vector<int> dimensions = getDimensions(data[0]);
    if (dimensions.size() != 2) {
        return 1;
    }
    int width = dimensions[0], height = dimensions[1];

    data.erase(data.begin());

    std::vector<std::string> plus, minus;
    for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        if ((*it)[0] == '+') {
            plus.push_back(it->substr(2));
        } 
        else if ((*it)[0] == '-') {
            minus.push_back(it->substr(2));
        }
    }

    char filler = findSafeFiller(minus);
    std::vector<std::string> grid(height, std::string(width, '.'));
    std::ofstream output(argv[2]);

    std::string command = argv[3];
    if (command == "one_solution") {
        if (onesol(grid, plus, 0, width, height)) {
            replaceDots(grid, filler);
            for (std::vector<std::string>::const_iterator it = grid.begin(); it != grid.end(); ++it) {
                output << *it << '\n';
            }
        } 
        else {
            output << "No solutions found\n";
        }
    } else if (command == "all_solutions") {
        std::set<std::vector<std::string>> uniqueSolutions;
        allsol(grid, plus, 0, width, height, uniqueSolutions);
        if (uniqueSolutions.empty()) {
            output << "No solutions found\n";
        } 
        else {
            output << uniqueSolutions.size() << " solution(s)\n";
            for (std::set<std::vector<std::string>>::const_iterator sol = uniqueSolutions.begin(); sol != uniqueSolutions.end(); ++sol) {
                output << "Board:\n";
                std::vector<std::string> displaySol = *sol;
                replaceDots(displaySol, filler);
                for (std::vector<std::string>::const_iterator it = displaySol.begin(); it != displaySol.end(); ++it) {
                    output << *it << "\n";
                }
            }
        }        
    } else {
        std::cerr << "Invalid keyword\n";
        return 1;
    }

    output.close();
}
