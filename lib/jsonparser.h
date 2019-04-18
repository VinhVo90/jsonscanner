#pragma once

#include <iostream>
#include "value.h"

using namespace std;

class JsonParser {
  string m_sContent;

public:
  JsonParser();
  ~JsonParser();

  static Value* parse(const string &sFileContent);
  static string parseAsBizFile(const string &sContent);
  // void print() const;

private:
  static void doParse(Array<string> arrContent, int nIndex, Value *pContainer);
  static string makeBaseString(const string &sContent);
  static bool detectStringValue(string &sContent, int &nIndex);
};

#include "jsonparser.cpp"