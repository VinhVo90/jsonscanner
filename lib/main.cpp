#include <iostream>
#include "jsonparser.h"
#include "common.h"
#include <string.h>
#include <regex>

using namespace std;

// doc_info format => doctype/OTPID/DTPID/msgtype/ref_no
void write_jsi(string biz_file_path, char* doc_info, string error_message) {
  if (!doc_info) return;
  
	char jsi_path[256] = { 0 };
	sprintf(jsi_path, "%s.jsi\0", biz_file_path.c_str());
	FILE* fp = fopen(jsi_path, "w");
	char* p_token = NULL == doc_info ? NULL : strtok(doc_info, "/");
	fprintf(fp, "doctype:%s\n", NULL == p_token ? "JSON" : p_token);
	fprintf(fp, "syntax_id:\n");
	fprintf(fp, "syntax_ver:\n");
	p_token = strtok(NULL, "/");
	fprintf(fp, "sender_id:%s\n", NULL == p_token ? "" : p_token);
	p_token = strtok(NULL, "/");
	fprintf(fp, "recipient_id:%s\n", NULL == p_token ? "" : p_token);
	p_token = strtok(NULL, "/");
	fprintf(fp, "msgtype:%s\n", NULL == p_token ? "" : p_token);
	p_token = strtok(NULL, "/");
	fprintf(fp, "msg_refnum:%s\n", NULL == p_token ? "" : p_token);
	fprintf(fp, "msg_ver:\n");
	fprintf(fp, "msg_rel:\n");
	fprintf(fp, "agency:\n");
	if (0 == error_message.length()) {
		fprintf(fp, "trans_status:C\n");
		fprintf(fp, "trans_result:NO_ERROR\n");
		fprintf(fp, "error_comment:\n");
	} else {
		fprintf(fp, "trans_status:E\n");
		fprintf(fp, "trans_result:ERROR\n");
		fprintf(fp, "error_comment:\n", error_message.c_str());
	}
	fclose(fp);
}

int main(int argc, char* argv[])
{
	if (3 > argc) {
		cout << "[ Usage ] jsonscanner json_file_path biz_file_path [doc_info<doctype/OTPID/DTPID/msgtype/ref_no>]\n\n";
		return 0;
	}

	string sJsonFilePath = argv[1];
	string sBizFilePath = argv[2];
	char* sDocInfo = (argc == 3) ? NULL : argv[3];

  

	string sFileContent = read_file(sJsonFilePath);
	if ("" == sFileContent) {
		write_jsi(sBizFilePath, sDocInfo, "Failed to read an input file");
		return -1;
	}

	JsonParser json;
	string sResult = json.parseAsBizFile(sFileContent);
  
	if ("" == sResult) {
		write_jsi(sBizFilePath, sDocInfo, json.getErrorMessage());
		return -1;
	}
  
	write_file(sBizFilePath, sResult);
	write_jsi(sBizFilePath.c_str(), sDocInfo, "");
	return 0;
}
