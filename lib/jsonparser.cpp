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
    int nDummy = 0;
    detectStringValue(sLine, nIndex1, nDummy);
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

string JsonParser::parse2(const string& sContent)
{
  list<string> lstBaseString = makeBaseString(sContent);
  if (0 == lstBaseString.size()) return "";

  list<string>::iterator it = lstBaseString.begin();
  Parent p;
  p.name = "ROOT";
  if ("{" == *it)
    p.type = OBJECT_TYPE::GROUP;
  else 
    p.type = OBJECT_TYPE::ARRAY;

  Array<Parent> arrParent;
  arrParent.insert(p);
  string sResult;
  for ( ; it != lstBaseString.end(); ++it) {
    doParse2(*it, arrParent, sResult);
  }

  return sResult;
}

void JsonParser::doParse2(const string& baseString, Array<Parent>& arrParent, string& result)
{
  string sLine = trim(baseString);

  if ("" == sLine) {
    return;
  }

  if ('}' == sLine[0] || ']' == sLine[0]) {
    // continue parsing for parent of current parent
    arrParent.removeLast();
    return;
  }

  const Parent parent = arrParent[arrParent.getCount() -1];
  if (OBJECT_TYPE::GROUP == parent.type) {
    // Group Items

    // Get item name
    int nIndex1 = 1;
    int nDummy = 0;
    detectStringValue(sLine, nIndex1, nDummy);
    string sAttrName = sLine.substr(1, nIndex1 - 1);

    // Get item value
    int nFound = sLine.find(':', nIndex1 + 1);
    string sAttrValue = trim(sLine.substr(nFound + 1));

    // Remove " from fist and last of sAttrValue
    if ('"' == sAttrValue[0]) sAttrValue.replace(0, 1, "");
    if ('"' == sAttrValue[sAttrValue.length() - 1]) sAttrValue.replace(sAttrValue.length() - 1, 1, "");

    if ("{" == sAttrValue) {
      // Item is Group
      Parent p;
      p.name = sAttrName;
      p.type = OBJECT_TYPE::GROUP;
      arrParent.insert(p);

      result.append("\n").append(sAttrName);
    } else if ("[" == sAttrValue) {
      // Item is Array
      Parent p;
      p.name = sAttrName;
      p.type = OBJECT_TYPE::ARRAY;
      arrParent.insert(p);
    } else {
      // Item is single
      result.append("\n").append(sAttrName).append(":").append(sAttrValue);
    }
  } else {
    // Array Items
    string sAttrValue = sLine;
    if ('"' == sAttrValue[0]) sAttrValue.replace(0, 1, "");
    if ('"' == sAttrValue[sAttrValue.length() - 1]) sAttrValue.replace(sAttrValue.length() - 1, 1, "");

    if ("{" == sAttrValue) {
      // Item is Group
      Parent p;
      p.name = parent.name + "Item";
      p.type = OBJECT_TYPE::GROUP;
      arrParent.insert(p);

      result.append("\n").append(parent.name);
      result.append("\n").append(parent.name).append("Item");
    } else if ("[" == sAttrValue) {
      // Item is Array
      Parent p;
      p.name = parent.name + "Item";
      p.type = OBJECT_TYPE::ARRAY;
      arrParent.insert(p);

      result.append("\n").append(parent.name);
      result.append("\n").append(parent.name).append("Item");
    } else {
      // Item is single
      result.append("\n").append(parent.name);
      result.append("\n").append(parent.name).append("Item").append(":").append(sAttrValue);
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
  int nOriginIndex = 0; // for error log
  int nIndex = 0;
  string sResult = sContent;
  list<string> lstResult;
  list<string> lstError;
  int nStartIndex = 0;

  skipWhiteSpace(sResult, nIndex, nOriginIndex);

  switch (sResult[nIndex]) {
    case '{':
      lstResult.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      if (!detectGroup(sResult, nIndex, nOriginIndex, lstResult)) {
        printError(sResult, nIndex, nOriginIndex);
        return lstError;
      }
      break;
    case '[':
      lstResult.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      if (!detectArray(sResult, nIndex, nOriginIndex, lstResult)) {
        printError(sResult, nIndex, nOriginIndex);
        return lstError;
      }
      break;
    default:
      printError(sResult, nIndex, nOriginIndex);
      return lstError;
      break;
  }

  // check if any more data after end of JSON
  // ex: {} aaa
  skipWhiteSpace(sResult, nIndex, nOriginIndex);
  if (nIndex < sResult.length()) {
    printError(sResult, nIndex, nOriginIndex);

    return lstError;
  }

  return lstResult;
}

bool JsonParser::detectGroup(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  while (true) {
    skipWhiteSpace(sContent, nIndex, nOriginIndex);

    if (!detectGroupItem(sContent, nIndex, nOriginIndex, lstBaseString)) {
      return false;
    }

    if (!nextGroupItem(sContent, nIndex, nOriginIndex, lstBaseString)) {
      // If there is no anymore item and there is no end of group charactor '}' => incorrect JSON format
      if (!endOfGroup(sContent, nIndex, nOriginIndex, lstBaseString)) return false;
      
      break;
    } 
  }

  nIndex += 1;
  nOriginIndex += 1;

  return true;
}

bool JsonParser::detectGroupItem(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  // Attribute name
  if ('"' != sContent[nIndex]) return false;
  nIndex += 1;
  nOriginIndex += 1;
  if (!detectStringValue(sContent, nIndex, nOriginIndex)) return false;

  nIndex += 1;
  nOriginIndex += 1;
  skipWhiteSpace(sContent, nIndex, nOriginIndex);
  if (':' != sContent[nIndex]) return false;

  // Attribute value
  nIndex += 1;
  nOriginIndex += 1;
  skipWhiteSpace(sContent, nIndex, nOriginIndex);
  switch (sContent[nIndex]) {
    case '{':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      return detectGroup(sContent, nIndex, nOriginIndex, lstBaseString);
      break;
    case '[':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      return detectArray(sContent, nIndex, nOriginIndex, lstBaseString);
      break;
    default:
      bool b = detectSingle(sContent, nIndex, nOriginIndex, lstBaseString);
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfGroup(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if ('}' == sContent[nIndex]) {
    lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextGroupItem(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString) {
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (',' == sContent[nIndex]) {
    nIndex += 1;
    nOriginIndex += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectArray(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  while (true) {
    skipWhiteSpace(sContent, nIndex, nOriginIndex);
    
    if (!detectArrayItem(sContent, nIndex, nOriginIndex, lstBaseString)) return false;

    if (!nextArrayItem(sContent, nIndex, nOriginIndex, lstBaseString)) {
      // If there is no anymore item and there is no end of array charactor ']' => incorrect JSON format
      if (!endOfArray(sContent, nIndex, nOriginIndex, lstBaseString)) return false;

      break;
    }
  }

  nIndex += 1;
  nOriginIndex += 1;

  return true;
}

bool JsonParser::detectArrayItem(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString) {
  int nStartIndex = nIndex;
  switch (sContent[nIndex]) {
    case '{':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      return detectGroup(sContent, nIndex, nOriginIndex, lstBaseString);
      break;
    case '[':
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      nOriginIndex += 1;
      return detectArray(sContent, nIndex, nOriginIndex, lstBaseString);
      break;
    default:
      bool b = detectSingle(sContent, nIndex, nOriginIndex, lstBaseString);
      lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfArray(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  int nStartIndex = nIndex;
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (']' == sContent[nIndex]) {
    lstBaseString.push_back(sContent.substr(nStartIndex, nIndex - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextArrayItem(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (',' == sContent[nIndex]) {
    nIndex += 1;
    nOriginIndex += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectSingle(string &sContent, int &nIndex, int &nOriginIndex, list<string>& lstBaseString)
{
  switch (sContent[nIndex]) {
    case '"':
      nIndex += 1;
      nOriginIndex += 1;
      if (!detectStringValue(sContent, nIndex, nOriginIndex)) return false;

      nIndex += 1;
      nOriginIndex += 1;
      return true;
      break;
    case 't':
    case 'f':
      if (!detectBoolValue(sContent, nIndex, nOriginIndex)) return false;

      nIndex += 1;
      nOriginIndex += 1;
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
      detectNumberValue(sContent, nIndex, nOriginIndex);
      return true;
      break;
    default:
      return false;
      break;
  }

  return false;
}

bool JsonParser::detectStringValue(string& sContent, int &nIndex, int &nOriginIndex)
{
  int nLen = sContent.length() - 1;

  while (nIndex < nLen && '\n' != sContent[nIndex] && !('"' == sContent[nIndex] && '\\' != sContent[nIndex - 1])) {
    nIndex += 1;
    nOriginIndex += 1;
  }

  if ('\n' == sContent[nIndex]) return false;
  if (nIndex == nLen) return false;

  return true;
}

bool JsonParser::detectBoolValue(string &sContent, int &nIndex, int &nOriginIndex)
{
  if ('t' == sContent[nIndex]) {
    nIndex += 1;
    nOriginIndex += 1;
    if ('r' != sContent[nIndex]) return false;

    nIndex += 1;
    nOriginIndex += 1;
    if ('u' != sContent[nIndex]) return false;

    nIndex += 1;
    nOriginIndex += 1;
    if ('e' != sContent[nIndex]) return false;
  } else {
    nIndex += 1;
    nOriginIndex += 1;
    if ('a' != sContent[nIndex]) return false;

    nIndex += 1;
    nOriginIndex += 1;
    if ('l' != sContent[nIndex]) return false;

    nIndex += 1;
    nOriginIndex += 1;
    if ('s' != sContent[nIndex]) return false;

    nIndex += 1;
    nOriginIndex += 1;
    if ('e' != sContent[nIndex]) return false;
  }

  return true;
}

void JsonParser::detectNumberValue(string &sContent, int &nIndex, int &nOriginIndex)
{
  if ('0' == sContent[nIndex]) {
    nIndex += 1;
    nOriginIndex += 1;
  } else {
    while (isdigit(sContent[nIndex])) {
      nIndex += 1;
      nOriginIndex += 1;
    }

    if ('.' == sContent[nIndex]) {
      nIndex += 1;
      nOriginIndex += 1;

      while (isdigit(sContent[nIndex])) {
        nIndex += 1;
        nOriginIndex += 1;
      }
    }
  }
}

void JsonParser::printError(const string &sContent, int nIndex, int nOriginIndex)
{
  cout << "SyntaxError: Unexpected token " << sContent[nIndex] << " in JSON at position " << nOriginIndex << endl;
}

void JsonParser::skipWhiteSpace(const string &sContent, int &nIndex, int &nOriginIndex)
{
  int nLen = sContent.length();
  while (nIndex < nLen) {
    switch (sContent[nIndex]) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        nIndex += 1;
        nOriginIndex += 1;
        break;
      default:
        return;
        break;
    }
  }
}

#endif