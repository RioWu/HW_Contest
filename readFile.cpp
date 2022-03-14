#include "readFile.h"
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>


vector<vector<string>> readFile::readCSV(string path)
{
    ifstream inFile(path, ios::in);  
    string lineStr;  
    vector<vector<string>> strArray;  
    while (getline(inFile, lineStr))  
    {  
         
        // 存成二维表结构  
        stringstream ss(lineStr);  
        string str;  
        vector<string> lineArray;  
        // 按照逗号分隔  
        while (getline(ss, str, ','))  
            lineArray.push_back(str);  
        strArray.push_back(lineArray);  
    }  

    return strArray;
} 

void readFile::readIni(string ini_file_path)
{
    ifstream inFile(ini_file_path, ios::in);
    string lineStr;  
    vector<string> strArray;  
    while (getline(inFile, lineStr))  
    {
        //cout << lineStr << endl;
        stringstream ss(lineStr);
        string str;
          
        while (getline(ss, str, '='))
            strArray.push_back(str);
    }
    this->qos_constraint = atoi(strArray.back().c_str());
    //cout << qos_constraint;
}

map<string, vector<string>> readFile::readQoS(string file_path)
{
    vector<vector<string>> arrStrQoS = readCSV(file_path);

    map<string, vector<string>> clientMp;
    int h = arrStrQoS.size();
    int len = arrStrQoS[0].size();

    for(int i = 1; i < len; i ++)
    {
        vector<string> tmp;
        clientMp[arrStrQoS[0][i]] = tmp;
    }
    for(int i = 1; i < len; i ++)
    {
        for(int j = 1; j < h; j ++)
        {
            int qos = atoi(arrStrQoS[j][i].c_str());
            //cout << qos << ' ';
            //cout << qos_constraint << endl;
            if(qos <= qos_constraint)
            {
                //cout << arrStrQoS[j][0];
                clientMp[arrStrQoS[0][i]].push_back(arrStrQoS[j][0]);
            }
            
        }
    }

    /*
    for(auto i : clientMp)
    {
        for(auto j : i.second)
        {
            cout << j << ' ';
        }
        cout << endl;
    }
    */
    return clientMp;
}

map<string, int> readFile::readSiteBandwidth(string file_path)
{
    vector<vector<string>> arrStrSev = readCSV(file_path);

    map<string, int> sevMp;

    for(int i = 1; i < arrStrSev.size(); i++)
    {
        sevMp[arrStrSev[i][0]] = atoi(arrStrSev[i][1].c_str());
    }

    /*
    for(auto  i : sevMp)
    {
        cout << i.first <<' '<< i.second<< endl;
    }
    */
    return sevMp;
}

map<string, vector<int>> readFile::readDemand(string file_path)
{
    vector<vector<string>> arrStrDemand = readCSV(file_path);

    map<string, vector<int>> demdMp;

    int h = arrStrDemand.size();
    int len = arrStrDemand[0].size();
    
    for(int i = 1; i < len; i ++)
    {
        vector<int> tmp;
        demdMp[arrStrDemand[0][i]] = tmp;
    }
    for(int i = 1; i < len; i ++)
    {
        for(int j = 1; j < h; j ++)
        {

            demdMp[arrStrDemand[0][i]].push_back(atoi(arrStrDemand[j][i].c_str()));
                
        }
    }


    return demdMp;
}