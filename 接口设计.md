# 接口设计

* 读文件

  ```cpp
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
  ```

* 流量分配算法

  1. 接口名称  LoadBalance
  2. 输入是readDemand readQoS readSiteBandwidth读取的数据
  3. 写入txt文件

  操作流程：

  根据客户端ID，读取这个时间点的流量请求，读取可访问的Server；根据**分配策略**进行流量分配，更新Server端的带宽（剩余带宽）。处理完一个客户端的流量请求，就写入txt。

  分配策略：

  * 贪心：直接将其分给剩余带宽最多的边缘结点

  \<CID, 流量>

