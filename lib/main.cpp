#include <iostream>
#include "jsonparser.h"
#include "common.h"
#include <string.h>
#include <regex>
using namespace std;

int main(int argc, char* argv[])
{
  // if (3 > argc) {
  //   cout << "[ Usage ] jsonparser json_file_path output_file_path\n\n";
  //   return 0;
  // }

  // string sInputFilePath = argv[1];
  // string sOutputFilePath = argv[2];
  string sInputFilePath ="D:\\Projects\\Ageta\\other_jobs\\20190410_json_parser\\app\\sample\\text1.txt";
  string sOutputFilePath = "D:\\Projects\\Ageta\\other_jobs\\20190410_json_parser\\app\\sample\\result.txt";

  string sFileContent = read_file(sInputFilePath);
  if ("" == sFileContent) {
    return 0;
  }

  JsonParser json;
  string sResult = json.parseAsBizFile(sFileContent);
  // string sResult = json.parse2(sFileContent);

  if ("" != sResult) {
    write_file(sOutputFilePath, sResult);
    cout << "Successfully." << endl;
  }

  return 0;
}