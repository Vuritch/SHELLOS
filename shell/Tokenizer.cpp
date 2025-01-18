#include "Tokenizer.h"
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

vector<string> Tokenizer::tokenize(const string& input) {
    vector<string> tokens;
    istringstream stream(input);
    string token;

    // Read space-separated tokens from the input string
    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}
