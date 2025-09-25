#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

std::vector<int> getDimensions(const std::string& line) {
    std::vector<int> dimensions;
    std::stringstream ss(line);
    int number;
    while (ss >> number) {
        dimensions.push_back(number);
    }
    return dimensions;
}

bool canPlace(const std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy, int width, int height) {
    int len = word.length();
    int endX = col + dx * (len - 1);
    int endY = row + dy * (len - 1);
    if (endX < 0 || endY < 0 || endX >= width || endY >= height) return false;

    for (int i = 0; i < len; i++) {
        if (grid[row][col] != '.' && grid[row][col] != word[i]) return false;
        row += dy;
        col += dx;
    }
    return true;
}

void placeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy) {
    for (char c : word) {
        grid[row][col] = c;
        row += dy;
        col += dx;
    }
}

void removeWord(std::vector<std::string>& grid, const std::string& word, int row, int col, int dx, int dy) {
    for (int i = 0; i < word.length(); i++) {
        grid[row][col] = '.';
        row += dy;
        col += dx;
    }
}

bool containsForbiddenWord(const std::vector<std::string>& grid, const std::set<std::string>& forbiddenWords, int row, int col, int width, int height) {
    std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    for (const auto& word : forbiddenWords) {
        int len = word.length();
        for (const auto& dir : directions) {
            int dx = dir.first, dy = dir.second;
            std::string candidate;
            int x = col, y = row;
            for (int i = 0; i < len; i++) {
                if (x < 0 || y < 0 || x >= width || y >= height) break;
                candidate += grid[y][x];
                x += dx;
                y += dy;
            }
            if (candidate == word) return true;
        }
    }
    return false;
}

bool findOneSolution(std::vector<std::string>& grid, const std::vector<std::string>& words, int index, int width, int height, const std::set<std::string>& forbiddenWords) {
    if (index == words.size()) {
        // Check if all positions are filled
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                if (grid[row][col] == '.') return false;  // There's still an empty space
            }
        }
        return true;  // All required words placed successfully
    }

    std::string word = words[index];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            for (auto direction : directions) {
                int dx = direction.first, dy = direction.second;
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::cout << "Placing word: " << word << " at (" << row << ", " << col << ")\n";
                    placeWord(grid, word, row, col, dx, dy);
                    if (findOneSolution(grid, words, index + 1, width, height, forbiddenWords)) {
                        return true;  // Solution found
                    }
                    removeWord(grid, word, row, col, dx, dy);  // Backtrack
                }
            }
        }
    }
    return false;  // No valid placement found
}

void fillDots(std::vector<std::string>& grid, int width, int height, std::set<std::vector<std::string>>& uniqueSolutions, const std::set<std::string>& forbiddenWords, int row, int col) {
    if (row >= height) return;  // End of grid reached

    while (col < width) {
        if (grid[row][col] == '.') {
            for (char c = 'a'; c <= 'z'; c++) {
                grid[row][col] = c;
                if (!containsForbiddenWord(grid, forbiddenWords, row, col, width, height)) {
                    fillDots(grid, width, height, uniqueSolutions, forbiddenWords, row, col + 1);  // Proceed to next cell
                }
                grid[row][col] = '.'; // Backtrack
            }
            return;  // Backtrack and continue to the next unfilled cell
        }
        col++;
    }

    fillDots(grid, width, height, uniqueSolutions, forbiddenWords, row + 1, 0);  // Move to next row
}

void findAllSolutions(std::vector<std::string>& grid, int width, int height, std::set<std::vector<std::string>>& uniqueSolutions, const std::set<std::string>& forbiddenWords, std::vector<std::string>& words, int wordIndex) {
    if (wordIndex == words.size()) {
        fillDots(grid, width, height, uniqueSolutions, forbiddenWords, 0, 0);
        return;
    }

    std::string word = words[wordIndex];
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            for (auto direction : directions) {
                int dx = direction.first, dy = direction.second;
                if (canPlace(grid, word, row, col, dx, dy, width, height)) {
                    std::cout << "Placing word: " << word << " at (" << row << ", " << col << ")\n";
                    placeWord(grid, word, row, col, dx, dy);
                    findAllSolutions(grid, width, height, uniqueSolutions, forbiddenWords, words, wordIndex + 1);
                    removeWord(grid, word, row, col, dx, dy);
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

    std::vector<int> dimensions = getDimensions(data[0]);
    int width = dimensions[0], height = dimensions[1];

    std::vector<std::string> words;
    std::set<std::string> forbiddenWords;
    for (const std::string& entry : data) {
        if (entry[0] == '+') words.push_back(entry.substr(2));
        else if (entry[0] == '-') forbiddenWords.insert(entry.substr(2));
    }

    std::vector<std::string> grid(height, std::string(width, '.'));
    std::set<std::vector<std::string>> uniqueSolutions;

    if (findOneSolution(grid, words, 0, width, height, forbiddenWords)) {
        std::cout << "Solution found!\n";
    } else {
        std::cout << "No solution found!\n";
    }

    return 0;
}
