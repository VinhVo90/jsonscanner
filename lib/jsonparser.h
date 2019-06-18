#pragma once

#include <iostream>
#include "value.h"
#include <list>
using namespace std;

class JsonParser {
  string m_sContent;
  string m_sErrorMessage;

public:
  JsonParser();
  ~JsonParser();

  Value* parse(const string &fileContent);
  string parseAsBizFile(const string &content);
  string getErrorMessage(void);

private:
  Value* doParse(const string& baseString, Value *container);
  list<string> makeBaseString(const string &content);

  bool detectGroup(string &content, int &index, list<string>& baseString);
  bool detectGroupItem(string &content, int &index, list<string>& baseString);
  bool endOfGroup(string &content, int &index, list<string>& baseString);
  bool nextGroupItem(string &content, int &index, list<string>& baseString);

  bool detectArray(string &content, int &index, list<string>& baseString);
  bool endOfArray(string &content, int &index, list<string>& baseString);
  bool detectArrayItem(string &content, int &index, list<string>& baseString);
  bool nextArrayItem(string &content, int &index, list<string>& baseString);

  bool detectSingle(string &content, int &index, list<string>& baseString);
  bool detectStringValue(string &content, int &index);
  bool detectBoolValue(string &content, int &index);
  void detectNumberValue(string &content, int &index);
  
  void skipWhiteSpace(const string &content, int &index);
  void printError(const string &content, int index);
};

#include "jsonparser.cpp"
