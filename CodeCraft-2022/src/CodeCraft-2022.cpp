//权重调度法
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <climits>
#include <cmath>
#include <sys/stat.h>
#include <sys/types.h>

// // for testing env
// #define CFG_PATH "/data/config.ini"
// #define QOS_PATH "/data/qos.csv"
// #define DMD_PATH "/data/demand.csv"
// #define BWT_PATH "/data/site_bandwidth.csv"
// #define OUT_PATH "/output/solution.txt"
// #define DIR_PATH "/output"

// for build_and_run.sh
// #define CFG_PATH "../data/config.ini"
// #define QOS_PATH "../data/qos.csv"
// #define DMD_PATH "../data/demand.csv"
// #define BWT_PATH "../data/site_bandwidth.csv"
// #define OUT_PATH "../output/solution.txt"
// #define DIR_PATH "../output"

// for vscode
#define CFG_PATH "../../data/config.ini"
#define QOS_PATH "../../data/qos.csv"
#define DMD_PATH "../../data/demand.csv"
#define BWT_PATH "../../data/site_bandwidth.csv"
#define OUT_PATH "../../output/solution.txt"
#define DIR_PATH "../../output"
using namespace std;
class Solution
{
private:
    int qos_constraint;
    int client_size;
    int site_size;
    int moment_size;

    // key:site name
    // value:the weight value for the key site
    map<string, float> site_mp;
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
    vector<pair<string, map<string, int>>> res;
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
            {
                char tmp = str.back();
                if (tmp == '\r' || tmp == '\n')
                {
                    str.pop_back();
                }
                if (str.back() == '\r')
                {
                    str.pop_back();
                }
                lineArray.push_back(str);
            }

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
    // set client_mp and site_mp
    void readQoS(string file_path)
    {
        {
            vector<vector<string>> arr_str_qos = readCSV(file_path);

            map<string, vector<string>> client_mp;
            map<string, float> site_mp;
            int h = arr_str_qos.size();
            int len = arr_str_qos[0].size();

            for (int i = 1; i < len; i++)
            {
                vector<string> tmp;
                client_mp[arr_str_qos[0][i]] = tmp;
            }
            // set client_mp
            for (int i = 1; i < len; i++)
            {
                for (int j = 1; j < h; j++)
                {
                    int qos = atoi(arr_str_qos[j][i].c_str());
                    if (qos < qos_constraint)
                    {
                        client_mp[arr_str_qos[0][i]].push_back(arr_str_qos[j][0]);
                    }
                }
            }
            // set site_mp
            for (int i = 1; i < h; i++)
            {
                float count = 0;
                for (int j = 1; j < len; j++)
                {
                    int qos = atoi(arr_str_qos[i][j].c_str());
                    if (qos <= qos_constraint)
                        count += 1;
                }
                if (count != 0)
                    site_mp[arr_str_qos[i][0]] = 1 / count;
            }
            this->client_size = client_mp.size();
            this->client_mp = client_mp;
            this->site_mp = site_mp;
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
        mkdir(DIR_PATH, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    }
    void loadAndBalance()
    {
        for (int i = 0; i < this->moment_size; i++)
        {
            map<string, int> tmp_bwt_mp = this->bwt_mp;
            //在每个时刻，依次遍历所有client并为它们计算出合理的流量分配方案
            for (auto it : this->client_mp)
            {
                string client = it.first;
                map<string, int> all_method;
                int demd_bwt = demd_mp[client][i];
                if (demd_bwt == 0)
                {
                    this->res.push_back({client, all_method});
                    continue;
                }
                // 每个循环内按权重进行一轮流量分配
                while (demd_bwt != 0)
                {
                    float sum = 0;
                    vector<string> ava_sites = it.second;
                    for (string s : ava_sites)
                    {
                        sum += this->site_mp[s];
                    }
                    // 将流量按权重依次为每一个节点进行分配
                    for (int j = 0; j < ava_sites.size(); j++)
                    {
                        string site_name = ava_sites[j];
                        // 向上取整，防止死循环
                        int tmp = ceil((site_mp[site_name] / sum) * demd_bwt);
                        // 该节点剩余可分配的流量大于要分配的流量，正常分配
                        if (tmp_bwt_mp[site_name] >= tmp)
                        {
                            if (tmp > demd_bwt)
                            {
                                tmp = demd_bwt;
                                demd_bwt = 0;
                                tmp_bwt_mp[site_name] -= demd_bwt;
                            }
                            else
                            {
                                demd_bwt -= tmp;
                                tmp_bwt_mp[site_name] -= tmp;
                            }
                        }
                        // 否则，尽可能的为该节点多分配流量，并将该节点从ava_sites中去除，再进行下一轮分配
                        else
                        {
                            tmp = tmp_bwt_mp[site_name];
                            if (tmp > demd_bwt)
                            {
                                tmp = demd_bwt;
                                demd_bwt = 0;
                                tmp_bwt_mp[site_name] -= demd_bwt;
                            }
                            else
                            {
                                demd_bwt -= tmp;
                                tmp_bwt_mp[site_name] = 0;
                                for (auto &it_tmp : this->client_mp)
                                {
                                    vector<string> ava_sites = it_tmp.second;
                                    remove(ava_sites.begin(), ava_sites.end(), site_name);
                                }
                            }
                        }
                        if (all_method.find(site_name) == all_method.end())
                            all_method[site_name] = tmp;
                        else
                            all_method[site_name] += tmp;
                    }
                }
                this->res.push_back({client, all_method});
            }
        }
    }
    void writeTxT()
    {
        ofstream out_file(OUT_PATH, ios::out);
        for (auto it = this->res.begin(); it != this->res.end(); it++)
        {
            string client = it->first;
            map<string, int> m = it->second;
            out_file << client + ":";
            if (m.size() == 0)
            {
                // 若分配方案为空，则只输出客户名称
            }
            // 分配方案中只有一个条目时，单独处理
            else if (m.size() == 1)
                out_file << "<" + m.begin()->first + "," + to_string(m.begin()->second) + ">";
            else
            {
                for (auto it_mp = m.begin(); it_mp != prev(m.end(), 1); it_mp++)
                {
                    out_file << "<" + it_mp->first + "," + to_string(it_mp->second) + ">" + ",";
                }
                // 分配方案输出时最后一个条目不需要加 ","，单独处理
                auto it_last = prev(m.end(), 1);
                out_file << "<" + it_last->first + "," + to_string(it_last->second) + ">";
            }
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
