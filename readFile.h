#ifndef READ_FILE_H
#define READ_FILE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

/*读取数据的类*/
class readFile
{
public:
    vector<vector<string>> readCSV(string path); //read csv file
    void readIni(string ini_file_path);  // read ini file, set qos_constraint
    map<string, vector<string>> readQoS(string file_path); // return <ClinetID, 可用边缘节点ID列表>

    map<string, vector<int>> readDemand(string file_path);  // return <ClinetID, 客户节点各个时刻的请求流量列表>

    map<string, int> readSiteBandwidth(string file_path);  // return <ServerID, 边缘节点的带宽>


private:
    int qos_constraint;
};



#endif