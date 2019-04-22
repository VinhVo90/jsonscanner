#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include "Array.h"
using namespace std;

string read_file(const string& file_path)
{
	ifstream file;
  string tmp;
  string res;
  file.open(file_path);
  if (!file) {
    cout << "Unable to open file";
    return "";
  }

  while(!file.eof()) {
    getline(file, tmp);
    res += tmp + "\n";
  }

  res.replace(res.length() - 1, 1, "");
  
	return res;
}

void write_file(const string& file_path, const string& content)
{
  FILE * pFile;
  pFile = fopen (file_path.c_str(),"w");

  if (!pFile) return;

  fputs (content.c_str(), pFile);
  fclose (pFile);
}

string replace_all(string str, const string& from, const string& to)
{
  size_t start_pos = 0;
  while (string::npos != (start_pos = str.find(from, start_pos))) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  
  return str;
}

Array<string> split(string s, string delimiter)
{
  Array<string> res;
  int pos = 0;
  string token;

  while (string::npos != (pos = s.find(delimiter))) {
    token = s.substr(0, pos);
    res.insertLast(token);
    s.erase(0, pos + delimiter.length());
  }

  res.insertLast(s);

  return res;
}

void ltrim(string &s)
{
  s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
    return !isspace(ch);
  }));
}

void rtrim(string &s)
{
  s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
    return !isspace(ch);
  }).base(), s.end());
}

string trim(string s)
{
  ltrim(s);
  rtrim(s);

  return s;
}