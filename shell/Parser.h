#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <utility>

// Structure to represent a command with its name and arguments
struct Command {
    std::string name;
    std::vector<std::string> arguments;
};

class Parser {
public:
    // Parse command tokens into a Command structure
    static Command parse(const std::vector<std::string>& tokens);

    // Parse a directory path into its parent path and directory name
    static std::pair<std::string, std::string> parsePath(const std::string& dirPath);
};

#endif // PARSER_H
