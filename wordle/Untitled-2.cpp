#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

// Convert string into vector of integers containing the dimensions
std::vector<int> getDimensions(const std::string& line) {
    std::vector<int> dimensions;
    std::stringstream ss(line);
    int number;
    while (ss >> number) {
        dimensions.push_back(number);
    }
    return dimensions;
}

// Check if we can place the word in the grid
bool canPlace(const std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, int width, int height) {
    int len = word.length();
    int endX = col + dx * (len - 1);
    int endY = row + dy * (len - 1);

    // Placement within border check
    if (endX < 0 || endY < 0 || endX >= width || endY >= height) return false;

    // Check if the word can be placed in the grid
    for (int i = 0; i < len; i++) {
        if (grid[row][col] != '.' && grid[row][col] != word[i]) return false;
        row += dy;
        col += dx;
    }
    return true;
}

// Place the word in the grid (after we check with canPlace)
std::vector<char> placeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy) {
    std::vector<char> originalChars;
    for (size_t i = 0; i < word.length(); i++) {
        originalChars.push_back(grid[row][col]);
        grid[row][col] = word[i];
        row += dy;
        col += dx;
    }
    return originalChars;
}

// Remove the word from the grid
void removeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, const std::vector<char>& originalChars) {
    for (size_t i = 0; i < word.size(); i++) {
        grid[row][col] = originalChars[i];
        row += dy;
        col += dx;
    }
}

// Check if the forbidden word is present in the grid 
bool isForbiddenWordPresent(const std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords) {
    int height = grid.size();
    int width = grid[0].size();

    for (const std::string& word : forbiddenWords) {
        int len = word.length();

        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                std::vector<std::pair<int, int>> directions = {
                    {1, 0}, {0, 1}, {-1, 0}, {0, -1},  
                    {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
                };

                for (std::pair<int, int> direction : directions) {
                    int dx = direction.first;
                    int dy = direction.second;

                    std::string candidate;
                    int x = col, y = row;

                    for (int i = 0; i < len; ++i) {
                        if (x < 0 || y < 0 || x >= width || y >= height) break;
                        candidate += grid[y][x];
                        x += dx;
                        y += dy;
                    }

                    if (candidate == word) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

// Helper function to check if placing a letter at (row, col) causes any forbidden words
// Helper function to check if placing a letter at (row, col) causes any forbidden words
bool canFillWithoutForbiddenWord(std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords, int row, int col) {
    int height = grid.size();
    int width = grid[0].size();
    for (const std::string& word : forbiddenWords) {
        int len = word.length();
        std::vector<std::pair<int, int>> directions = {
            {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
        };
        for (std::pair<int, int> direction : directions) {
            int dx = direction.first;
            int dy = direction.second;
            std::string candidate;
            int x = col, y = row;
            for (int i = 0; i < len; ++i) {
                if (x < 0 || y < 0 || x >= width || y >= height) break;
                candidate += grid[y][x];
                x += dx;
                y += dy;
            }
            if (candidate == word) {
                return false;  // Forbidden word found
            }
        }
    }
    return true;  // No forbidden words found
}

// Backtracking function to fill empty spaces (no return type)
// Function to check if placing a letter at (row, col) causes any forbidden words
bool isValidFill(const std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords, int row, int col) {
    int height = grid.size();
    int width = grid[0].size();
    
    // Check all directions for forbidden words
    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };

    for (const std::string& word : forbiddenWords) {
        int len = word.length();

        for (const auto& direction : directions) {
            int dx = direction.first;
            int dy = direction.second;
            std::string candidate;
            int x = col, y = row;
            
            for (int i = 0; i < len; ++i) {
                if (x < 0 || y < 0 || x >= width || y >= height) break;
                candidate += grid[y][x];
                x += dx;
                y += dy;
            }

            if (candidate == word) {
                return false;  // Forbidden word found
            }
        }
    }

    return true;  // No forbidden words found
}

// Backtracking function to fill empty spaces with valid letters
void fillEmptySpaces(std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords) {
    int height = grid.size();
    int width = grid[0].size();

    // Find the first empty cell
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            if (grid[row][col] == '.') {  // We have an empty cell
                // Try placing each letter from 'a' to 'z' in the empty cell
                for (char letter = 'a'; letter <= 'z'; ++letter) {
                    grid[row][col] = letter;  // Place the letter

                    // Check if this placement causes any forbidden words
                    if (isValidFill(grid, forbiddenWords, row, col)) {
                        // If valid, continue filling the remaining empty spaces recursively
                        fillEmptySpaces(grid, forbiddenWords);

                        // Check if the grid is fully filled and no forbidden word exists
                        if (std::count(grid[row].begin(), grid[row].end(), '.') == 0) {
                            return;  // Solution found, exit early
                        }
                    }
                    grid[row][col] = '.';  // Backtrack if this letter didn't work
                }
                return;  // No valid letter could be placed, backtrack
            }
        }
    }
}


// Find a single solution
bool findOneSolution(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, const std::set<std::string>& forbiddenWords) {
    if (index == words.size()) {
        fillEmptySpaces(grid, forbiddenWords);
        return !isForbiddenWordPresent(grid, forbiddenWords);
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {
                {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
            };
            for (std::pair<int, int> direction : directions) {
                int dx = direction.first;
                int dy = direction.second;

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

// Find all solutions
void findAllSolutions(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, std::set<std::vector<std::string>>& uniqueSolutions, const std::set<std::string>& forbiddenWords) {

    if (index == words.size()) {
        fillEmptySpaces(grid, forbiddenWords);
        if(!isForbiddenWordPresent(grid, forbiddenWords)) {
            uniqueSolutions.insert(grid);
        }
        return;
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {
                {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
            };
            for (std::pair<int, int> direction : directions) {
                int dx = direction.first;
                int dy = direction.second;

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
            output << "Board:\n";
            for (const std::string& row : grid) output << row << "\n";
        } else {
            output << "No solutions found\n";
        }
    } else if (command == "all_solutions") {
        std::set<std::vector<std::string>> uniqueSolutions;
        findAllSolutions(grid, plus, 0, width, height, uniqueSolutions, forbiddenWords);
        output << uniqueSolutions.size() << " solution(s)\n";
        for (const std::vector<std::string>& sol : uniqueSolutions) {
            output << "Board:\n";
            for (const std::string& row : sol) output << row << "\n";
        }
    } else {
        std::cerr << "Invalid keyword\n";
        return 1;
    }

    output.close();
    return 0;
}
