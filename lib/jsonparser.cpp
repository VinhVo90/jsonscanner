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
  string sTmp = sContent;
  sTmp = makeBaseString(sTmp);
  if ("" == sTmp) return NULL;

  Array<string> arrContent = split(sTmp, "\n");

  // Remove all empty lines
  int nCount = arrContent.getCount();
  for (int i = nCount - 1; i >= 0; i -= 1) {
    if ("" == trim(arrContent[i])) {
      arrContent.remove(i);
    }
  }

  Value *pResult;
  if ("{" == arrContent[0])
    pResult = new GroupValue("ROOT");
  else 
    pResult = new ArrayValue("ROOT");

  doParse(arrContent, 1, pResult);

  return pResult;
}

void JsonParser::doParse(Array<string> arrContent, int nIndex, Value* pContainer)
{
  if (nIndex == arrContent.getCount()) return;
  if (!pContainer) return;

  string sLine = arrContent[nIndex];

  sLine = trim(sLine);

  if ("" == sLine) {
    doParse(arrContent, nIndex + 1, pContainer);
    return;
  }

  if ('}' == sLine[0] || ']' == sLine[0]) {
    doParse(arrContent, nIndex + 1, pContainer->getContainer());
    return;
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
      doParse(arrContent, nIndex + 1, item);
    } else if ("[" == sAttrValue) {
      item = new ArrayValue(sAttrName, pContainer);
      ((GroupValue*)pContainer)->add(item);
      doParse(arrContent, nIndex + 1, item);
    } else {
      item = new SingleValue(sAttrName, sAttrValue, pContainer);
      ((GroupValue*)pContainer)->add(item);
      doParse(arrContent, nIndex + 1, pContainer);
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
      doParse(arrContent, nIndex + 1, item);
    } else {
      item = new SingleValue(pContainer->getName() + "Item", sAttrValue, pContainer);
      ((ArrayValue*)pContainer)->add(item);
      doParse(arrContent, nIndex + 1, pContainer);
    }
  }
}

string JsonParser::parseAsBizFile(const string& sContent)
{
  GroupValue *pRoot = (GroupValue*)parse(sContent);

  if (!pRoot) return "";

  string sResult;
  int nCount = pRoot->getItemCount();
  for(int i = 0; i < nCount; i += 1) {
    sResult.append(pRoot->getItemByIndex(i)->toBizFileString());
  }

  delete pRoot;

  return sResult;
}

string JsonParser::makeBaseString(const string& sContent)
{
  int nLen = sContent.length();
  int nOriginIndex = 0; // for error log
  int nIndex = 0;
  string sResult = sContent;

  skipWhiteSpace(sResult, nIndex, nOriginIndex);

  switch (sResult[nIndex]) {
    case '{':
      sResult.replace(nIndex, 1, "{\n");
      nIndex += 2;
      nOriginIndex += 1;
      if (!detectGroup(sResult, nIndex, nOriginIndex)) {
        printError(sResult, nIndex, nOriginIndex);
        return "";
      }
      break;
    case '[':
      sResult.replace(nIndex, 1, "[\n");
      nIndex += 2;
      nOriginIndex += 1;
      if (!detectArray(sResult, nIndex, nOriginIndex)) {
        printError(sResult, nIndex, nOriginIndex);
        return "";
      }
      break;
    default:
      printError(sResult, nIndex, nOriginIndex);
      return "";
      break;
  }

  // check if any more data after end of JSON
  // ex: {} aaa
  skipWhiteSpace(sResult, nIndex, nOriginIndex);
  if (nIndex < sResult.length()) {
    printError(sResult, nIndex, nOriginIndex);

    return "";
  }

  return sResult;
}

bool JsonParser::detectGroup(string &sContent, int &nIndex, int &nOriginIndex)
{
  while (!endOfGroup(sContent, nIndex, nOriginIndex)) {
    if (!detectGroupItem(sContent, nIndex, nOriginIndex)) {
      return false;
    }
    
    if (!nextGroupItem(sContent, nIndex, nOriginIndex)) return false;
  }

  nIndex += 1;
  nOriginIndex += 1;

  return true;
}

bool JsonParser::detectGroupItem(string &sContent, int &nIndex, int &nOriginIndex)
{
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
      sContent.replace(nIndex, 1, "{\n");
      nIndex += 2;
      nOriginIndex += 1;
      return detectGroup(sContent, nIndex, nOriginIndex);
      break;
    case '[':
      sContent.replace(nIndex, 1, "[\n");
      nIndex += 2;
      nOriginIndex += 1;
      return detectArray(sContent, nIndex, nOriginIndex);
      break;
    default:
      return detectSingle(sContent, nIndex, nOriginIndex);
      break;
  }
}

bool JsonParser::endOfGroup(string &sContent, int &nIndex, int &nOriginIndex)
{
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if ('}' == sContent[nIndex]) {
    sContent.replace(nIndex, 1, "\n}");
    nIndex += 1;

    return true;
  }
  
  return false;
}

bool JsonParser::nextGroupItem(string &sContent, int &nIndex, int &nOriginIndex) {
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (',' == sContent[nIndex]) {
    sContent.replace(nIndex, 1, "\n");
    nIndex += 1;
    nOriginIndex += 1;

    return true;
  }

  if (!endOfGroup(sContent, nIndex, nOriginIndex)) return false;

  return true;
}

bool JsonParser::detectArray(string &sContent, int &nIndex, int &nOriginIndex)
{
  while (!endOfArray(sContent, nIndex, nOriginIndex)) {
    if (!detectArrayItem(sContent, nIndex, nOriginIndex)) {
      return false;
    }
    
    if (!nextArrayItem(sContent, nIndex, nOriginIndex)) return false;
  }

  nIndex += 1;
  nOriginIndex += 1;

  return true;
}

bool JsonParser::detectArrayItem(string &sContent, int &nIndex, int &nOriginIndex) {
  switch (sContent[nIndex]) {
    case '{':
      sContent.replace(nIndex, 1, "{\n");
      nIndex += 2;
      nOriginIndex += 1;
      return detectGroup(sContent, nIndex, nOriginIndex);
      break;
    case '[':
      sContent.replace(nIndex, 1, "[\n");
      nIndex += 2;
      nOriginIndex += 1;
      return detectArray(sContent, nIndex, nOriginIndex);
      break;
    default:
      return detectSingle(sContent, nIndex, nOriginIndex);
      break;
  }
}

bool JsonParser::endOfArray(string &sContent, int &nIndex, int &nOriginIndex)
{
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (']' == sContent[nIndex]) {
    sContent.replace(nIndex, 1, "\n]");
    nIndex += 1;

    return true;
  }
  
  return false;
}

bool JsonParser::nextArrayItem(string &sContent, int &nIndex, int &nOriginIndex)
{
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (',' == sContent[nIndex]) {
    sContent.replace(nIndex, 1, "\n");
    nIndex += 1;
    nOriginIndex += 1;

    return true;
  }

  if (!endOfArray(sContent, nIndex, nOriginIndex)) return false;

  return true;
}

bool JsonParser::detectSingle(string &sContent, int &nIndex, int &nOriginIndex)
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