#pragma once
#include <string>
#include <vector>
using namespace std;

class Utility
{
public:
	// split string
	static void split(const string& s, vector<string>& tokens, const string& delimiters = " ");
	// replace substring 
	static void replace(string& src, string from, string to);
};