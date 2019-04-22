#pragma once

#include <iostream>
#include "value.h"

using namespace std;

class JsonParser {
  string m_sContent;

public:
  JsonParser();
  ~JsonParser();

  Value* parse(const string &sFileContent);
  string parseAsBizFile(const string &sContent);

private:
  void doParse(Array<string> arrContent, int nIndex, Value *pContainer);
  string makeBaseString(const string &sContent);

  bool detectGroup(string &sContent, int &nIndex, int &nOriginIndex);
  bool detectGroupItem(string &sContent, int &nIndex, int &nOriginIndex);
  bool endOfGroup(string &sContent, int &nIndex, int &nOriginIndex);
  bool nextGroupItem(string &sContent, int &nIndex, int &nOriginIndex);

  bool detectArray(string &sContent, int &nIndex, int &nOriginIndex);
  bool endOfArray(string &sContent, int &nIndex, int &nOriginIndex);
  bool detectArrayItem(string &sContent, int &nIndex, int &nOriginIndex);
  bool nextArrayItem(string &sContent, int &nIndex, int &nOriginIndex);

  bool detectSingle(string &sContent, int &nIndex, int &nOriginIndex);
  bool detectStringValue(string &sContent, int &nIndex, int &nOriginIndex);
  bool detectBoolValue(string &sContent, int &nIndex, int &nOriginIndex);
  void detectNumberValue(string &sContent, int &nIndex, int &nOriginIndex);
  
  void skipWhiteSpace(const string &sContent, int &nIndex, int &nOriginIndex);
  void printError(const string &sContent, int nIndex, int nOriginIndex);
};

#include "jsonparser.cpp"