#ifndef GRAPH
#define GRAPH

#include"gurobi_c++.h"
#include"ComMethod.h"
#include"Show.h"
#include<utility>
#include<vector>
using std::vector;
using std::pair;



class Graph {
public:
	Graph(GRBModel a) : model(a) { show = new Save; };
	~Graph() {
		if (show != NULL)
			delete show;
		if (method != NULL)
			delete method;
	}
	int get_d() { return d_tot; }
	bool check();//检查是否正确
	void translator();//输出规范的网络流文件
	bool load(int i);//读入第i个样例文件
	bool load(string name);//读入名字为name的文件
	void setN(int N) { d_tot = N; }//设置层数N
	void compute();//计算
	void Init(int i);//初始化网络流图
	Result save() {//保存结果
		show = new Save;
		return (*show)(d_tot, result);
	}
	void print() {//打印结果
		show = new Print;
		(*show)(d_tot, result);
	}
	void getflow() {//打印网络流
		show = new GetFlow;
		(*show)(d_tot, model);
	}
	void setmethod(ComMethod* a) {//设置算法
		method = a;
		result.clear();
		count = 0;
	}
	void clear() {
		result.clear();
		TCS.clear();
	}
private:
	vector<Result> result;//连续计算的结果数组
	Show* show;//关于打印
	GRBModel model;//GRB线性规划模型
	ComMethod* method;//算法策略
	double costs;//初始溶液的权值
	double costb;//缓冲溶液的权值
	int d_tot;//总层数
	int used[1025];//辅助数组
	int count = 0;
	vector<pair<int, int>>TCS;//记录目标。first为目标浓度，second为目标数量

	GRBVar f(int d, int a, int b);//返回由第d层的第a个节点，指向第d+1层第b个节点的边
	void Constrain();//更新限制
	int pow2(int d);//pow（2，d）
	void Record();//记录结果与result数组
	void Remove();//删去已经不需要的限制
};


inline GRBVar Graph::f(int d,int a,int b){
  string name = "floor"+to_string(d)+"_"+to_string(a)+"_"+to_string(b);
  if(d==-1)
    name = "source" + to_string(b);
	if (d == d_tot) {
		if (b == 0)
			name = "waste_" + to_string(a);
		if (b == 1)
			name = "target_" + to_string(a);
	}
  return model.getVarByName(name);
}


inline int Graph::pow2(int d){
  int result = 1;
  for(int i=0;i<d;i++)
    result*=2;
  return result;
}

#endif
