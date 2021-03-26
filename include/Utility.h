#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
using namespace std;

class Utility
{
public:
	// split string
	static void split(const string& s, vector<string>& tokens, const string& delimiters = " ");
	// replace substring 
	static void replace(string& src, string from, string to);
	// get file name's suffix
	static string getSuffix(const string& fileName);
	// print glm vec
	static void printVec(const glm::vec3 &v);
};