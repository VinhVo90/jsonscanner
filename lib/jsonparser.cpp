#ifndef _JSON_PARSER_CPP
#define _JSON_PARSER_CPP

#include "jsonparser.h"
#include "singlevalue.h"
#include "arrayvalue.h"
#include "groupvalue.h"
#include "value.h"
#include "common.h"



JsonParser::JsonParser()
{
}

JsonParser::~JsonParser()
{
}

Value* JsonParser::parse(const string& sContent)
{
  list<string> lstBaseString = makeBaseString(sContent);
  if (0 == lstBaseString.size()) return NULL;

  list<string>::iterator it = lstBaseString.begin();
  Value *pResult;
  if ("{" == *it)
    pResult = new GroupValue("ROOT");
  else 
    pResult = new ArrayValue("ROOT");

  Value* pTmp = pResult;
  ++it;
  for ( ; it != lstBaseString.end(); ++it) {
    pTmp = doParse(*it, pTmp);
  }

  return pResult;
}

Value* JsonParser::doParse(const string& baseString, Value *pContainer)
{
  string sLine = trim(baseString);

  if ("" == sLine) {
    return pContainer;
  }

  if ('}' == sLine[0] || ']' == sLine[0]) {
    return pContainer->getContainer();
  }

  if (OBJECT_TYPE::GROUP == pContainer->getType()) {
    // Group Items
    int nIndex1 = 1;
    detectStringValue(sLine, nIndex1);
    string sAttrName = trim(sLine.substr(1, nIndex1));

    int nFound = sLine.find(':', nIndex1 + 1);
    string sAttrValue = trim(sLine.substr(nFound + 1));

    if ('"' == sAttrName[0]) sAttrName.replace(0, 1, "");
    if ('"' == sAttrName[sAttrName.length() - 1]) sAttrName.replace(sAttrName.length() - 1, 1, "");

    if ('"' == sAttrValue[0]) sAttrValue.replace(0, 1, "");
    if ('"' == sAttrValue[sAttrValue.length() - 1]) sAttrValue.replace(sAttrValue.length() - 1, 1, "");

    Value *item;
    if ("{" == sAttrValue) {
      item = new GroupValue(sAttrName, pContainer);
      ((GroupValue*)pContainer)->add(item);
      return item;

    } else if ("[" == sAttrValue) {
      item = new ArrayValue(sAttrName, pContainer);
      ((GroupValue*)pContainer)->add(item);
      return item;

    } else {
      item = new SingleValue(sAttrName, sAttrValue, pContainer);
      ((GroupValue*)pContainer)->add(item);
      return pContainer;
    }
  } else {
    // Array Items
    string sAttrValue = sLine;
    if ('"' == sAttrValue[0]) sAttrValue.replace(0, 1, "");
    if ('"' == sAttrValue[sAttrValue.length() - 1]) sAttrValue.replace(sAttrValue.length() - 1, 1, "");
    Value *item;
    if ("{" == sAttrValue) {
      item = new GroupValue(pContainer->getName() + "Item", pContainer);
      ((ArrayValue*)pContainer)->add(item);
      return item;
    } else {
      item = new SingleValue(pContainer->getName() + "Item", sAttrValue, pContainer);
      ((ArrayValue*)pContainer)->add(item);
      return pContainer;
    }
  }
}

string JsonParser::parseAsBizFile(const string& sContent)
{
  GroupValue *pRoot = (GroupValue*)parse(sContent);

  if (!pRoot) return "";

  string sResult;
  pRoot->forEach([&](Value* item, int index){
    sResult.append(item->toBizFileString());
  });
  
  delete pRoot;

  return sResult;
}

list<string> JsonParser::makeBaseString(const string& sContent)
{
  int nLen = sContent.length();
  int nIndex = 0;
  string sResult = sContent;
  list<string> lstResult;
  list<string> lstError;

  skipWhiteSpace(sResult, nIndex);

  int nStartIndex = nIndex;
  switch (sResult[nIndex]) {
    case '{':
      lstResult.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      if (!detectGroup(sResult, nIndex, lstResult)) {
        printError(sResult, nIndex);
        return lstError;
      }
      break;
    case '[':
      lstResult.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      if (!detectArray(sResult, nIndex, lstResult)) {
        printError(sResult, nIndex);
        return lstError;
      }
      break;
    default:
      printError(sResult, nIndex);
      return lstError;
      break;
  }

  // check if any more data after end of JSON
  // ex: {} aaa
  skipWhiteSpace(sResult, nIndex);
  if (nIndex < sResult.length()) {
    printError(sResult, nIndex);

    return lstError;
  }

  return lstResult;
}

bool JsonParser::detectGroup(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  while (true) {
    skipWhiteSpace(sContent, nIndex);

    if (!detectGroupItem(sContent, nIndex, lstBaseString)) {
      return false;
    }

    if (!nextGroupItem(sContent, nIndex, lstBaseString)) {
      // If there is no anymore item and there is no end of group charactor '}' => incorrect JSON format
      if (!endOfGroup(sContent, nIndex, lstBaseString)) return false;
      
      break;
    } 
  }

  nIndex += 1;

  return true;
}

bool JsonParser::detectGroupItem(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  // Attribute name
  if ('"' != sContent[nIndex]) return false;
  nIndex += 1;
  if (!detectStringValue(sContent, nIndex)) return false;

  nIndex += 1;
  skipWhiteSpace(sContent, nIndex);
  if (':' != sContent[nIndex]) return false;

  // Attribute value
  nIndex += 1;
  skipWhiteSpace(sContent, nIndex);
  switch (sContent[nIndex]) {
    case '{':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      return detectGroup(sContent, nIndex, lstBaseString);
      break;
    case '[':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      return detectArray(sContent, nIndex, lstBaseString);
      break;
    default:
      bool b = detectSingle(sContent, nIndex, lstBaseString);
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfGroup(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  skipWhiteSpace(sContent, nIndex);

  if ('}' == sContent[nIndex]) {
    lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextGroupItem(string &sContent, int &nIndex, list<string>& lstBaseString) {
  skipWhiteSpace(sContent, nIndex);

  if (',' == sContent[nIndex]) {
    nIndex += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectArray(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  while (true) {
    skipWhiteSpace(sContent, nIndex);
    
    if (!detectArrayItem(sContent, nIndex, lstBaseString)) return false;

    if (!nextArrayItem(sContent, nIndex, lstBaseString)) {
      // If there is no anymore item and there is no end of array charactor ']' => incorrect JSON format
      if (!endOfArray(sContent, nIndex, lstBaseString)) return false;

      break;
    }
  }

  nIndex += 1;

  return true;
}

bool JsonParser::detectArrayItem(string &sContent, int &nIndex, list<string>& lstBaseString) {
  int nStartIndex = nIndex;
  switch (sContent[nIndex]) {
    case '{':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      return detectGroup(sContent, nIndex, lstBaseString);
      break;
    case '[':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      return detectArray(sContent, nIndex, lstBaseString);
      break;
    default:
      bool b = detectSingle(sContent, nIndex, lstBaseString);
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfArray(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  skipWhiteSpace(sContent, nIndex);

  if (']' == sContent[nIndex]) {
    lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextArrayItem(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  skipWhiteSpace(sContent, nIndex);

  if (',' == sContent[nIndex]) {
    nIndex += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectSingle(string &sContent, int &nIndex, list<string>& lstBaseString)
{
  switch (sContent[nIndex]) {
    case '"':
      nIndex += 1;
      if (!detectStringValue(sContent, nIndex)) return false;

      nIndex += 1;
      return true;
      break;
    case 't':
    case 'f':
      if (!detectBoolValue(sContent, nIndex)) return false;

      nIndex += 1;
      return true;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      detectNumberValue(sContent, nIndex);
      return true;
      break;
    default:
      return false;
      break;
  }

  return false;
}

bool JsonParser::detectStringValue(string& sContent, int &nIndex)
{
  int nLen = sContent.length() - 1;

  while (nIndex < nLen && '\n' != sContent[nIndex] && !('"' == sContent[nIndex] && '\\' != sContent[nIndex - 1])) {
    nIndex += 1;
  }

  if ('\n' == sContent[nIndex]) return false;
  if (nIndex == nLen) return false;

  return true;
}

bool JsonParser::detectBoolValue(string &sContent, int &nIndex)
{
  if ('t' == sContent[nIndex]) {
    nIndex += 1;
    if ('r' != sContent[nIndex]) return false;

    nIndex += 1;
    if ('u' != sContent[nIndex]) return false;

    nIndex += 1;
    if ('e' != sContent[nIndex]) return false;
  } else {
    nIndex += 1;
    if ('a' != sContent[nIndex]) return false;

    nIndex += 1;
    if ('l' != sContent[nIndex]) return false;

    nIndex += 1;
    if ('s' != sContent[nIndex]) return false;

    nIndex += 1;
    if ('e' != sContent[nIndex]) return false;
  }

  return true;
}

void JsonParser::detectNumberValue(string &sContent, int &nIndex)
{
  if ('0' == sContent[nIndex]) {
    nIndex += 1;
  } else {
    while (isdigit(sContent[nIndex])) {
      nIndex += 1;
    }

    if ('.' == sContent[nIndex]) {
      nIndex += 1;

      while (isdigit(sContent[nIndex])) {
        nIndex += 1;
      }
    }
  }
}

void JsonParser::printError(const string &sContent, int nIndex)
{
  cout << "SyntaxError: Unexpected token " << sContent[nIndex] << " in JSON at position " << nIndex << endl;
}

void JsonParser::skipWhiteSpace(const string &sContent, int &nIndex)
{
  int nLen = sContent.length();
  while (nIndex < nLen) {
    switch (sContent[nIndex]) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        nIndex += 1;
        break;
      default:
        return;
        break;
    }
  }
}

#endif