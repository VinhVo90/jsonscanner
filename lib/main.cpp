#include <iostream>
#include "jsonparser.h"
#include "common.h"
#include <string.h>
using namespace std;

int main(int argc, char* argv[]) {

  // if (argc < 3) {
  //   cout << "[ Usage ] jsonparser json_file_path output_file_path\n\n";
  //   return 0;
  // }

  // string sInputFilePath = argv[1];
  // string sOutputFilePath = argv[2];

  string sInputFilePath = "D:\\Projects\\Ageta\\other_jobs\\20190410_json_parser\\app\\sample\\text.txt";
  string sOutputFilePath = "D:\\Projects\\Ageta\\other_jobs\\20190410_json_parser\\app\\sample\\result.txt";

  string sFileContent = read_file(sInputFilePath);
  if ("" == sFileContent) {
    return 0;
  }

  string sResult = JsonParser::parseAsBizFile(sFileContent);

  if ("" != sResult) {
    write_file(sOutputFilePath, sResult);
  }

  return 0;
}