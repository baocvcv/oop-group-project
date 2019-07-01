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
	bool check();//����Ƿ���ȷ
	void translator();//����淶���������ļ�
	bool load(int i);//�����i�������ļ�
	bool load(string name);//��������Ϊname���ļ�
	void setN(int N) { d_tot = N; }//���ò���N
	void compute();//����
	void Init(int i);//��ʼ��������ͼ
	Result save() {//������
		show = new Save;
		return (*show)(d_tot, result);
	}
	void print() {//��ӡ���
		show = new Print;
		(*show)(d_tot, result);
	}
	void getflow() {//��ӡ������
		show = new GetFlow;
		(*show)(d_tot, model);
	}
	void setmethod(ComMethod* a) {//�����㷨
		method = a;
		result.clear();
		count = 0;
	}
	void clear() {
		result.clear();
		TCS.clear();
	}
private:
	vector<Result> result;//��������Ľ������
	Show* show;//���ڴ�ӡ
	GRBModel model;//GRB���Թ滮ģ��
	ComMethod* method;//�㷨����
	double costs;//��ʼ��Һ��Ȩֵ
	double costb;//������Һ��Ȩֵ
	int d_tot;//�ܲ���
	int used[1025];//��������
	int count = 0;
	vector<pair<int, int>>TCS;//��¼Ŀ�ꡣfirstΪĿ��Ũ�ȣ�secondΪĿ������

	GRBVar f(int d, int a, int b);//�����ɵ�d��ĵ�a���ڵ㣬ָ���d+1���b���ڵ�ı�
	void Constrain();//��������
	int pow2(int d);//pow��2��d��
	void Record();//��¼�����result����
	void Remove();//ɾȥ�Ѿ�����Ҫ������
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
