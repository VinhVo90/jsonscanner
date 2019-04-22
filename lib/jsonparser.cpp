#ifndef _JSON_PARSER_CPP
#define _JSON_PARSER_CPP

#include "jsonparser.h"
#include "singlevalue.h"
#include "arrayvalue.h"
#include "groupvalue.h"
#include "value.h"
#include "common.h"

JsonParser::JsonParser() {
}

JsonParser::~JsonParser() {
}

Value* JsonParser::parse(const string& sContent) {
  string sTmp = sContent;

  sTmp = makeBaseString(sTmp);
  if ("" == sTmp) return NULL;

  Array<string> arrContent = split(sTmp, "\n");

  GroupValue *result = new GroupValue("ROOT");

  doParse(arrContent, 1, result);

  return result;
}

void JsonParser::doParse(Array<string> arrContent, int nIndex, Value* pContainer) {
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

  Array<string> attribute = split(sLine, ":");

  if (attribute.getCount() > 1) {
    // Group Items
    string sAttrName = trim(attribute[0]);
    if ('"' == sAttrName[0]) sAttrName.replace(0,1,"");
    if ('"' == sAttrName[sAttrName.length() - 1]) sAttrName.replace(sAttrName.length() - 1, 1, "");

    string sAttrValue = trim(attribute[1]);
    if ('"' == sAttrValue[0]) sAttrValue.replace(0,1,"");
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
    string sAttrValue = trim(attribute[0]);
    if ('"' == sAttrValue[0]) sAttrValue.replace(0,1,"");
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

string JsonParser::parseAsBizFile(const string& sContent) {
  GroupValue *pRoot = (GroupValue*)parse(sContent);

  if (!pRoot) return "";

  // communication
  GroupValue *pCommunication;
  SingleValue *pSender, *pRecipient, *pReferenceId;

  pCommunication = (GroupValue*)pRoot->getItemByName("communication");
  if (!pCommunication) {
    cout << "Cannot find communication group.";
    return "";
  }
  
  pSender = (SingleValue*)pCommunication->getItemByName("sender");
  if (!pSender) {
    cout << "Cannot find communication->sender attribute.";
    return "";
  }

  pRecipient = (SingleValue*)pCommunication->getItemByName("recipient");
  if (!pRecipient) {
    cout << "Cannot find communication->recipient attribute.";
    return "";
  }

  pReferenceId = (SingleValue*)pCommunication->getItemByName("referenceId");
  if (!pReferenceId) {
    cout << "Cannot find communication->referenceId attribute.";
    return "";
  }

  // communication->message
  GroupValue *pMessage;
  SingleValue *pMessageName, *pMessageVersion, *pMessageRelease, *pMessageAgency;

  pMessage = (GroupValue*)pCommunication->getItemByName("message");
  if (!pMessage) {
    cout << "Cannot find communication->message group.";
    return "";
  }

  pMessageName = (SingleValue*)pMessage->getItemByName("name");
  if (!pMessageName) {
    cout << "Cannot find communication->message->name attribute.";
    return "";
  }

  pMessageVersion = (SingleValue*)pMessage->getItemByName("version");
  if (!pMessageVersion) {
    cout << "Cannot find communication->message->version attribute.";
    return "";
  }

  pMessageRelease = (SingleValue*)pMessage->getItemByName("release");
  if (!pMessageRelease) {
    cout << "Cannot find communication->message->release attribute.";
    return "";
  }

  pMessageAgency = (SingleValue*)pMessage->getItemByName("agency");
  if (!pMessageAgency) {
    cout << "Cannot find communication->message->agency attribute.";
    return "";
  }

  // communication->keys
  ArrayValue* pKeys = (ArrayValue*)pCommunication->getItemByName("keys");
  if (!pKeys) {
    cout << "Cannot find communication->keys array.";
    return "";
  }

  // payload
  GroupValue* pPayload = (GroupValue*)((GroupValue*)pRoot)->getItemByName("payload");
  if (!pPayload) {
    cout << "Cannot find payload group.";
    return "";
  }

  // Make output string
  string sResult = "DOC_BEGIN\n";

  // HEADER
  sResult.append("HEADER\n");

  sResult.append(pSender->toBizFileString());
  sResult.append(pRecipient->toBizFileString());

  // HEADER -> message
  sResult.append("messageName:").append(pMessageName->getValue()).append("\n");
  sResult.append("messageVersion:").append(pMessageVersion->getValue()).append("\n");
  sResult.append("messageRelease:").append(pMessageRelease->getValue()).append("\n");
  sResult.append("messageAgency:").append(pMessageAgency->getValue()).append("\n");

  sResult.append(pReferenceId->toBizFileString());

  // HEADER -> keys
  int nCount = pKeys->getItemCount();
  for( int i = 0; i < nCount; i += 1) {
    sResult.append("key").append(to_string(i + 1)).append(":").append(((SingleValue*)pKeys->getItemByIndex(i))->getValue()).append("\n");
  }

  // payload
  nCount = pPayload->getItemCount();

  for (int i = 0; i < nCount; i += 1) {
    sResult.append(pPayload->getItemByIndex(i)->toBizFileString());
  }

  sResult.append("DOC_END");

  delete pRoot;

  return sResult;
}

string JsonParser::makeBaseString(const string& sContent) {
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

bool JsonParser::detectGroup(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::detectGroupItem(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::endOfGroup(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::detectArray(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::endOfArray(string &sContent, int &nIndex, int &nOriginIndex) {
  skipWhiteSpace(sContent, nIndex, nOriginIndex);

  if (']' == sContent[nIndex]) {
    sContent.replace(nIndex, 1, "\n]");
    nIndex += 1;

    return true;
  }
  
  return false;
}

bool JsonParser::nextArrayItem(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::detectSingle(string &sContent, int &nIndex, int &nOriginIndex) {
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

bool JsonParser::detectStringValue(string& sContent, int &nIndex, int &nOriginIndex) {
  int nLen = sContent.length() - 1;

  while (nIndex < nLen && '\n' != sContent[nIndex] && !('"' == sContent[nIndex] && '\\' != sContent[nIndex - 1])) {
    nIndex += 1;
    nOriginIndex += 1;
  }

  if ('\n' == sContent[nIndex]) return false;
  if (nIndex == nLen) return false;

  return true;
}

bool JsonParser::detectBoolValue(string &sContent, int &nIndex, int &nOriginIndex) {
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

void JsonParser::detectNumberValue(string &sContent, int &nIndex, int &nOriginIndex) {
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

void JsonParser::printError(const string &sContent, int nIndex, int nOriginIndex) {
  cout << "SyntaxError: Unexpected token " << sContent[nIndex] << " in JSON at position " << nOriginIndex << endl;
}

void JsonParser::skipWhiteSpace(const string &sContent, int &nIndex, int &nOriginIndex) {
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

// void JsonParser::print() const {
//   // Value *root = parse();

//   // root->print();

//   // delete root;
// }

#endif