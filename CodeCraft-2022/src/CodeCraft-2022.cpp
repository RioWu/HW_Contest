#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <climits>
#define CFG_PATH "/data/config.ini"
#define QOS_PATH "/data/qos.csv"
#define DMD_PATH "/data/demand.csv"
#define BWT_PATH "/data/site_bandwidth.csv"
#define OUT_PATH "/output/solution.txt"
using namespace std;
class Solution
{
private:
    int qos_constraint;
    int client_size;
    int site_size;
    int moment_size;
    // key:client name
    // value: all available sites for the key client
    map<string, vector<string>> client_mp;
    // key:client name
    // value:demand bandwidth seque for the key client
    map<string, vector<int>> demd_mp;
    // key:site name
    // value:bandwidth for the key site
    map<string, int> bwt_mp;
    // key:client name
    // value:sequence of <site name,bandwidth allocated to the site>
    vector<pair<string, vector<map<string, int>>>> res_mp;
    // helper function
    // read csv file
    vector<vector<string>> readCSV(string path)
    {
        ifstream inFile(path, ios::in);
        string lineStr;
        vector<vector<string>> strArray;
        while (getline(inFile, lineStr))
        {
            stringstream ss(lineStr);
            string str;
            vector<string> lineArray;
            // seperate by comma
            while (getline(ss, str, ','))
                lineArray.push_back(str);
            strArray.push_back(lineArray);
        }

        return strArray;
    }

public:
    // read ini file
    // set qos_constraint
    void readIni(string ini_file_path)
    {
        ifstream inFile(ini_file_path, ios::in);
        string lineStr;
        vector<string> strArray;
        while (getline(inFile, lineStr))
        {
            // cout << lineStr << endl;
            stringstream ss(lineStr);
            string str;

            while (getline(ss, str, '='))
                strArray.push_back(str);
        }
        this->qos_constraint = atoi(strArray.back().c_str());
    }
    // read qos.csv
    // set client_mp
    void readQoS(string file_path)
    {
        {
            vector<vector<string>> arr_str_qos = readCSV(file_path);

            map<string, vector<string>> client_mp;
            int h = arr_str_qos.size();
            int len = arr_str_qos[0].size();

            for (int i = 1; i < len; i++)
            {
                vector<string> tmp;
                client_mp[arr_str_qos[0][i]] = tmp;
            }
            for (int i = 1; i < len; i++)
            {
                for (int j = 1; j < h; j++)
                {
                    int qos = atoi(arr_str_qos[j][i].c_str());
                    if (qos <= qos_constraint)
                    {
                        client_mp[arr_str_qos[0][i]].push_back(arr_str_qos[j][0]);
                    }
                }
            }
            this->client_size = client_mp.size();
            this->client_mp = client_mp;
        }
    }
    // read demand.csv
    // set demd_mp
    void readDemand(string file_path)
    {
        vector<vector<string>> arr_str_demand = readCSV(file_path);

        map<string, vector<int>> demd_mp;

        int h = arr_str_demand.size();
        int len = arr_str_demand[0].size();

        for (int i = 1; i < len; i++)
        {
            vector<int> tmp;
            demd_mp[arr_str_demand[0][i]] = tmp;
        }
        for (int i = 1; i < len; i++)
        {
            for (int j = 1; j < h; j++)
            {

                demd_mp[arr_str_demand[0][i]].push_back(atoi(arr_str_demand[j][i].c_str()));
            }
        }
        this->moment_size = demd_mp.begin()->second.size();
        this->demd_mp = demd_mp;
    }
    // read site_bandwidth.csv
    // set bwt_mp
    void readSiteBandwidth(string file_path)
    {
        vector<vector<string>> arr_str_bwt = readCSV(file_path);

        map<string, int> bwt_mp;

        for (int i = 1; i < arr_str_bwt.size(); i++)
        {
            bwt_mp[arr_str_bwt[i][0]] = atoi(arr_str_bwt[i][1].c_str());
        }
        this->site_size = bwt_mp.size();
        this->bwt_mp = bwt_mp;
    }
    // init
    void init()
    {
        this->readIni(CFG_PATH);
        this->readDemand(DMD_PATH);
        this->readQoS(QOS_PATH);
        this->readSiteBandwidth(BWT_PATH);
    }
    void loadAndBalance()
    {
        for (int i = 0; i < moment_size; i++)
        {
            map<string, int> tmp_bwt_mp = this->bwt_mp;
            for (auto it = this->client_mp.begin(); it != this->client_mp.end(); it++)
            {
                string opt_site;
                int max = INT_MIN;
                string client = it->first;
                int all_bwt = demd_mp.find(client)->second[i];
                vector<string> ava_sites = it->second;
                for (auto j : ava_sites)
                {
                    if (bwt_mp[j] > max)
                    {
                        max = bwt_mp[j];
                        opt_site = j;
                    }
                }
                this->bwt_mp[opt_site] -= all_bwt;
                this->res_mp.push_back({client, {{{opt_site, all_bwt}}}});
            }
        }
    }
    void writeTxT()
    {
        ofstream out_file(OUT_PATH, ios::out);
        for (auto it = this->res_mp.begin(); it != this->res_mp.end(); it++)
        {
            string client = it->first;
            vector<map<string, int>> v = it->second;
            out_file << client + ":";
            for (int i = 0; i < v.size() - 1; i++)
            {
                auto it = v[i].begin();
                out_file << "<" + it->first + "," + to_string(it->second) + ">" + ",";
            }
            auto it_last = v[v.size() - 1].begin();
            out_file << "<" + it_last->first + "," + to_string(it_last->second) + ">";
            out_file << "\r\n";
        }
        out_file.close();
    }
};
int main(int argc, char const *argv[])
{
    Solution s;
    // read data files
    s.init();
    s.loadAndBalance();
    s.writeTxT();
    return 0;
}
