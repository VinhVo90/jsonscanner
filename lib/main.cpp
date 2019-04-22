#include <iostream>
#include "jsonparser.h"
#include "common.h"
#include <string.h>
#include <regex>
using namespace std;

int main(int argc, char* argv[]) {

  if (3 > argc) {
    cout << "[ Usage ] jsonparser json_file_path output_file_path\n\n";
    return 0;
  }

  string sInputFilePath = argv[1];
  string sOutputFilePath = argv[2];

  string sFileContent = read_file(sInputFilePath);
  if ("" == sFileContent) {
    return 0;
  }

  JsonParser json;
  string sResult = json.parseAsBizFile(sFileContent);

  if ("" != sResult) {
    write_file(sOutputFilePath, sResult);
    cout << "Successfully." << endl;
  }

  return 0;
}