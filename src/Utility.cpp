#include "..\include\Utility.h"

void Utility::split(const string & s, vector<string>& tokens, const string & delimiters)
{
    string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

void Utility::replace(string & src, string from, string to)
{
    size_t pos = src.find(from);
    if (pos == string::npos)
        return;
    src.replace(pos, from.length(), to);
}
