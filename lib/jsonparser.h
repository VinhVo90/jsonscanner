#pragma once

#include <iostream>
#include "value.h"
#include <list>
using namespace std;

class JsonParser {
  string m_sContent;

public:
  JsonParser();
  ~JsonParser();

  Value* parse(const string &sFileContent);
  string parseAsBizFile(const string &sContent);

private:
  Value* doParse(const string& baseString, Value *pContainer);
  list<string> makeBaseString(const string &sContent);

  bool detectGroup(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool detectGroupItem(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool endOfGroup(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool nextGroupItem(string &sContent, int &nIndex, list<string>& lstBaseString);

  bool detectArray(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool endOfArray(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool detectArrayItem(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool nextArrayItem(string &sContent, int &nIndex, list<string>& lstBaseString);

  bool detectSingle(string &sContent, int &nIndex, list<string>& lstBaseString);
  bool detectStringValue(string &sContent, int &nIndex);
  bool detectBoolValue(string &sContent, int &nIndex);
  void detectNumberValue(string &sContent, int &nIndex);
  
  void skipWhiteSpace(const string &sContent, int &nIndex);
  void printError(const string &sContent, int nIndex);
};

#include "jsonparser.cpp"