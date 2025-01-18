#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <utility>
using namespace std;
// Structure to represent a command with its name and arguments
struct Command {
     string name;
     vector< string> arguments;
};

class Parser {
public:
    // Parse command tokens into a Command structure
    static Command parse(const  vector< string>& tokens);

    // Parse a directory path into its parent path and directory name
    static  pair< string,  string> parsePath(const  string& dirPath);
};

#endif // PARSER_H
