/*
 *This code make no sense(Compiled successfully),
 *but for exercise1_23 in <<K&R>> to test remove all comments in C code.
 */

#include "xlog.h"

#include <string>
#include <fstream>

using namespace std;


extern void rmComment(string str, string& strOut);

void funzdfds1(){/*
dfds//dfs
//dfd
*///dfgdsg
string str="RmComment test/*dfsd*///1234";/*dfdsfv*///dfsd
}


static string strTest="", strstand = "";

static void initrmStr(string& strTest){
    strTest += "void funzdfds1(){/*\n";
    strTest += "dfds//dfs\n";
    strTest += "//dfd\n";
    strTest += "*///dfgdsg\n";
    strTest += "string str=\"RmComment test/*dfsd*///1234\";/*dfdsfv*///dfsd\n";
    strTest += "}\n";

    strstand += "void funzdfds1(){\n";
    strstand += "string str=\"RmComment test/*dfsd*///1234\";\n";
    strstand += "}\n";
}

int RmComment_test(int argc, char const *argv[]){
    initrmStr(strTest);
    string strOut;
    rmComment(strTest, strOut);
    if(strOut != strstand){
        LOG_DBG("RmComment test err\n");
    }
    else{
        LOG_INFO("RmComment test succ\n%s", strOut.c_str());
    }

    return 0;
}





// //这个程序在做的是将一个n行1列的文本数据转化为n/7行7列的数据，每个数据项之间用","分隔。
// int main()
// {
// 	ifstream srcFile("E:\\Users\\data.txt", ios::in); //以文本模式打开txt文件
// 	ofstream outFile("E:\\Users\\output.txt", ios::out); //以文本模式创建输出文件
// 	string x;
// 	int flag = 0;
// 	while (getline(srcFile, x))    //文本读取方式
// 	{
// 		outFile << x;
// 		flag++;
// 		if (flag % 7 == 0) {
// 			outFile << endl;
// 			flag = 0;
// 		}
// 		else {
// 			outFile << ",";
// 		}
// 	}
// 	outFile.close();
// 	srcFile.close();
// 	return 0;
// }



