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

string JsonParser::makeBaseString(const string& sContent) {
  int nLen = sContent.length();
  int nOriginIndex = 0, nTmp; // for error log
  int nIndex = 0;
  bool bFlag = true;
  string sResult = sContent;

  while(nIndex < nLen) {
    cout << sResult[nIndex] << endl;
    switch (sResult[nIndex]) {
      case '{':
        sResult.replace(nIndex, 1, "{\n");
        nIndex += 2;
        nOriginIndex += 1;
        nLen += 1;
        break;
      case '}':
        sResult.replace(nIndex, 1, "\n}");
        nIndex += 2;
        nOriginIndex += 1;
        nLen += 1;
        break;
      case '[':
        sResult.replace(nIndex, 1, "[\n");
        nIndex += 2;
        nOriginIndex += 1;
        nLen += 1;
        break;
      case ']':
        sResult.replace(nIndex, 1, "\n]");
        nIndex += 2;
        nOriginIndex += 1;
        nLen += 1;
        break;
      case ',':
        sResult.replace(nIndex, 1, "\n");
        nIndex += 1;
        nOriginIndex += 1;
        break;
      case '"':
        nTmp = nIndex;
        bFlag = detectStringValue(sResult, nIndex);
        nOriginIndex = nOriginIndex + nIndex - nTmp;
        if (!bFlag) {
          // cout << "SyntaxError: Unexpected token " << sResult[nIndex] << " in JSON at position " << nOriginIndex << endl;
          cout << "Incorrect JSON format.";
          return "";
        }
        break;
      case ' ':
      case '\t':
      case ':':
      case '\n':
      case '\r':
        nIndex += 1;
        nOriginIndex += 1;
        break;
      default:
        // cout << "SyntaxError: Unexpected token " << sResult[nIndex] << " in JSON at position " << nOriginIndex << endl;
        cout << "Incorrect JSON format.";
        return "";
        break;
    }
  }

  return sResult;
}

bool JsonParser::detectStringValue(string& sContent, int &nIndex) {
  int nLen = sContent.length() - 1;
  nIndex += 1;

  while (nIndex < nLen && '\n' != sContent[nIndex] && !('"' == sContent[nIndex] && '\\' != sContent[nIndex - 1])) {
    nIndex += 1;
  }

  if ('\n' == sContent[nIndex]) return false;
  if (nIndex == nLen) return false;

  nIndex += 1;

  return true;
}

void JsonParser::doParse(Array<string> arrContent, int nIndex, Value* pContainer) {
  if (nIndex == arrContent.getCount() - 1) return;
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
  Value *root = parse(sContent);

  if (!root) return "";

  string sResult = "DOC_BEGIN\n";

  // HEADER
  sResult.append("HEADER\n");
  GroupValue *com = (GroupValue*)((GroupValue*) root)->getItemByName("communication");
  sResult.append(com->getItemByName("sender")->toBizFileString());
  sResult.append(com->getItemByName("recipient")->toBizFileString());

  // HEADER -> message
  GroupValue* mess = (GroupValue*)com->getItemByName("message");
  sResult.append("messageName:").append(((SingleValue*)mess->getItemByName("name"))->getValue()).append("\n");
  sResult.append("messageVersion:").append(((SingleValue*)mess->getItemByName("version"))->getValue()).append("\n");
  sResult.append("messageRelease:").append(((SingleValue*)mess->getItemByName("release"))->getValue()).append("\n");
  sResult.append("messageAgency:").append(((SingleValue*)mess->getItemByName("agency"))->getValue()).append("\n");

  sResult.append(com->getItemByName("referenceId")->toBizFileString());

  // HEADER -> keys
  ArrayValue* keys = (ArrayValue*)com->getItemByName("keys");
  int nCount = keys->getItemCount();
  for( int i = 0; i < nCount; i += 1) {
    sResult.append("key").append(to_string(i + 1)).append(":").append(((SingleValue*)keys->getItemByIndex(i))->getValue()).append("\n");
  }

  // payload
  GroupValue* payload = (GroupValue*)((GroupValue*)root)->getItemByName("payload");
  nCount = payload->getItemCount();

  for (int i = 0; i < nCount; i += 1) {
    sResult.append(payload->getItemByIndex(i)->toBizFileString());
  }

  sResult.append("DOC_END");

  delete root;

  return sResult;
}

// void JsonParser::print() const {
//   // Value *root = parse();

//   // root->print();

//   // delete root;
// }

#endif