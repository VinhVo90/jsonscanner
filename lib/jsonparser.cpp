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

string JsonParser::getErrorMessage(void) {
	return m_sErrorMessage;
}

Value* JsonParser::parse(const string& content)
{
  list<string> lstBaseString = makeBaseString(content);
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

Value* JsonParser::doParse(const string& baseString, Value *container)
{
  string sLine = trim(baseString);

  if ("" == sLine) {
    return container;
  }

  if ('}' == sLine[0] || ']' == sLine[0]) {
    return container->getContainer();
  }

  if (OBJECT_TYPE::GROUP == container->getType()) {
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
      item = new GroupValue(sAttrName, container);
      ((GroupValue*)container)->add(item);
      return item;

    } else if ("[" == sAttrValue) {
      item = new ArrayValue(sAttrName, container);
      ((GroupValue*)container)->add(item);
      return item;

    } else {
      item = new SingleValue(sAttrName, sAttrValue, container);
      ((GroupValue*)container)->add(item);
      return container;
    }
  } else {
    // Array Items
    string sAttrValue = sLine;
    if ('"' == sAttrValue[0]) sAttrValue.replace(0, 1, "");
    if ('"' == sAttrValue[sAttrValue.length() - 1]) sAttrValue.replace(sAttrValue.length() - 1, 1, "");
    Value *item;
    if ("{" == sAttrValue) {
      item = new GroupValue(container->getName() + "Item", container);
      ((ArrayValue*)container)->add(item);
      return item;
    } else {
      item = new SingleValue(container->getName() + "Item", sAttrValue, container);
      ((ArrayValue*)container)->add(item);
      return container;
    }
  }
}

string JsonParser::parseAsBizFile(const string& content)
{
  GroupValue *pRoot = (GroupValue*)parse(content);

  if (!pRoot) return "";

  string sResult;
  pRoot->forEach([&](Value* item, int index){
    sResult.append(item->toBizFileString());
  });
  
  delete pRoot;

  return sResult;
}

list<string> JsonParser::makeBaseString(const string& content)
{
  int nLen = content.length();
  int nIndex = 0;
  string sResult = content;
  list<string> lstResult;
  list<string> lstError;

  skipWhiteSpace(sResult, nIndex);

  int nStartIndex = nIndex;
  switch (sResult[nIndex]) {
    case '{':
      lstResult.push_back(content.substr(nStartIndex, nIndex - nStartIndex + 1));
      nIndex += 1;
      if (!detectGroup(sResult, nIndex, lstResult)) {
        printError(sResult, nIndex);
        return lstError;
      }
      break;
    case '[':
      lstResult.push_back(content.substr(nStartIndex, nIndex - nStartIndex + 1));
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

bool JsonParser::detectGroup(string &content, int &index, list<string>& baseString)
{
  while (true) {
    skipWhiteSpace(content, index);

    if (!detectGroupItem(content, index, baseString)) {
      return false;
    }

    if (!nextGroupItem(content, index, baseString)) {
      // If there is no anymore item and there is no end of group charactor '}' => incorrect JSON format
      if (!endOfGroup(content, index, baseString)) return false;
      
      break;
    } 
  }

  index += 1;

  return true;
}

bool JsonParser::detectGroupItem(string &content, int &index, list<string>& baseString)
{
  int nStartIndex = index;
  // Attribute name
  if ('"' != content[index]) return false;
  index += 1;
  if (!detectStringValue(content, index)) return false;

  index += 1;
  skipWhiteSpace(content, index);
  if (':' != content[index]) return false;

  // Attribute value
  index += 1;
  skipWhiteSpace(content, index);
  switch (content[index]) {
    case '{':
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));
      index += 1;
      return detectGroup(content, index, baseString);
      break;
    case '[':
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));
      index += 1;
      return detectArray(content, index, baseString);
      break;
    default:
      bool b = detectSingle(content, index, baseString);
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfGroup(string &content, int &index, list<string>& baseString)
{
  int nStartIndex = index;
  skipWhiteSpace(content, index);

  if ('}' == content[index]) {
    baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextGroupItem(string &content, int &index, list<string>& baseString) {
  skipWhiteSpace(content, index);

  if (',' == content[index]) {
    index += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectArray(string &content, int &index, list<string>& baseString)
{
  while (true) {
    skipWhiteSpace(content, index);
    
    if (!detectArrayItem(content, index, baseString)) return false;

    if (!nextArrayItem(content, index, baseString)) {
      // If there is no anymore item and there is no end of array charactor ']' => incorrect JSON format
      if (!endOfArray(content, index, baseString)) return false;

      break;
    }
  }

  index += 1;

  return true;
}

bool JsonParser::detectArrayItem(string &content, int &index, list<string>& baseString) {
  int nStartIndex = index;
  switch (content[index]) {
    case '{':
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));
      index += 1;
      return detectGroup(content, index, baseString);
      break;
    case '[':
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));
      index += 1;
      return detectArray(content, index, baseString);
      break;
    default:
      bool b = detectSingle(content, index, baseString);
      baseString.push_back(content.substr(nStartIndex, index - nStartIndex));
      return b;
      break;
  }
}

bool JsonParser::endOfArray(string &content, int &index, list<string>& baseString)
{
  int nStartIndex = index;
  skipWhiteSpace(content, index);

  if (']' == content[index]) {
    baseString.push_back(content.substr(nStartIndex, index - nStartIndex + 1));

    return true;
  }
  
  return false;
}

bool JsonParser::nextArrayItem(string &content, int &index, list<string>& baseString)
{
  skipWhiteSpace(content, index);

  if (',' == content[index]) {
    index += 1;

    return true;
  }

  return false;
}

bool JsonParser::detectSingle(string &content, int &index, list<string>& baseString)
{
  switch (content[index]) {
    case '"':
      index += 1;
      if (!detectStringValue(content, index)) return false;

      index += 1;
      return true;
      break;
    case 't':
    case 'f':
      if (!detectBoolValue(content, index)) return false;

      index += 1;
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
      detectNumberValue(content, index);
      return true;
      break;
    default:
      return false;
      break;
  }

  return false;
}

bool JsonParser::detectStringValue(string& content, int &index)
{
  int nLen = content.length() - 1;

  while (index < nLen && '\n' != content[index] && !('"' == content[index] && '\\' != content[index - 1])) {
    index += 1;
  }

  if ('\n' == content[index]) return false;
  if (index == nLen) return false;

  return true;
}

bool JsonParser::detectBoolValue(string &content, int &index)
{
  if ('t' == content[index]) {
    index += 1;
    if ('r' != content[index]) return false;

    index += 1;
    if ('u' != content[index]) return false;

    index += 1;
    if ('e' != content[index]) return false;
  } else {
    index += 1;
    if ('a' != content[index]) return false;

    index += 1;
    if ('l' != content[index]) return false;

    index += 1;
    if ('s' != content[index]) return false;

    index += 1;
    if ('e' != content[index]) return false;
  }

  return true;
}

void JsonParser::detectNumberValue(string &content, int &index)
{
  if ('0' == content[index]) {
    index += 1;
  } else {
    while (isdigit(content[index])) {
      index += 1;
    }

    if ('.' == content[index]) {
      index += 1;

      while (isdigit(content[index])) {
        index += 1;
      }
    }
  }
}

void JsonParser::printError(const string &content, int index)
{
  m_sErrorMessage = "SyntaxError: Unexpected token ";
  m_sErrorMessage += content[index];
  m_sErrorMessage += " in JSON at position ";
  m_sErrorMessage += to_string(index + 1);
  cout << m_sErrorMessage << endl;
}

void JsonParser::skipWhiteSpace(const string &content, int &index)
{
  int nLen = content.length();
  while (index < nLen) {
    switch (content[index]) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        index += 1;
        break;
      default:
        return;
        break;
    }
  }
}

#endif
