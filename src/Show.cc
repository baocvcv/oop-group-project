#include"include/Show.h"
#include<cstring>
#include<cmath>
#include<iostream>
#include<string>
using namespace std;


Result Save::operator()(const int d_tot,const vector<Result>result){
	double aveNs = 0, aveNb = 0, aveNw = 0, aveNd = 0;
	for (int i = 0; i < (int)result.size(); i++) {
		aveNs += result[i].Ns;
		aveNb += result[i].Nb;
		aveNw += result[i].Nw;
		aveNd += result[i].Nd;
	}
	aveNs /= (double)result.size();
	aveNb /= (double)result.size();
	aveNw /= (double)result.size();
	aveNd /= (double)result.size();

	Result ans;
  ans.N = d_tot;
	ans.Ns = aveNs;
	ans.Nb = aveNb;
	ans.Nw = aveNw;
	ans.Nd = aveNd;
	return ans;
}

Result Print::operator()(const int d_tot,const vector<Result>result){
  double aveNs = 0, aveNb = 0, aveNw = 0, aveNd = 0;
  for (int i = 0; i < (int)result.size(); i++) {
    aveNs += result[i].Ns;
    aveNb += result[i].Nb;
    aveNw += result[i].Nw;
    aveNd += result[i].Nd;
  }
  aveNs /= (double)result.size();
  aveNb /= (double)result.size();
  aveNw /= (double)result.size();
  aveNd /= (double)result.size();

  cout<<d_tot<<std::endl;
  cout<<aveNs<<std::endl;
  cout<<aveNb<<std::endl;
  cout<<aveNw<<std::endl;
  cout<<aveNd<<std::endl;

	return Result();
}

void GetFlow::operator()(const int d_tot,GRBModel& m) {
	cout << "//////////////////////////////////////////////////" << endl;
	int source1 = (int)m.getVarByName("source1").get(GRB_DoubleAttr_X);
	int source0 = (int)m.getVarByName("source0").get(GRB_DoubleAttr_X);
  int*** f;
  f = new int**[10];
  for(int i=0;i<10;i++){
    f[i] = new int*[1025];
    for(int j=0;j<1025;j++)
      f[i][j] = new int[1025];
  }

	for (int i = 0; i < d_tot; i++) {
		int point1 = pow(2, i) + 1;
		int point2 = pow(2, i + 1) + 1;
		string name;
		for (int j = 0; j < point2; j++) {
			if (j < point1) {
				for (int k = 0; k <= j; k++) {
					name = "floor" + to_string(i) + "_" +
						to_string(k) + "_" + to_string(j);
					f[i][k][j] = (int)m.getVarByName(name).get(GRB_DoubleAttr_X);
				}
			}
			else {
				for (int k = point1 - 1; k >= j - point1 + 1; k--) {
					name = "floor" + to_string(i) + "_" +
						to_string(k) + "_" + to_string(j);
					f[i][k][j] = (int)m.getVarByName(name).get(GRB_DoubleAttr_X);
				}
			}
		}
	}


	for (int i = d_tot - 1; i >= 0; i--) {
		int point1 = pow(2, i) + 1;
		int point2 = pow(2, i + 1) + 1;
		int base = pow(2, d_tot - 1 - i);
		cout << "floor" << to_string(i + 1) << ": ";

		int node[1024];
		memset(node, 0, sizeof(node));
		for (int j = 0; j < point1; j++)
			for (int k = 0; k < point2; k++) {
				node[k] += f[i][j][k];
			}
		for (int j = 0; j < point2; j++) {
			cout << node[j] << "*" << j * base << " ";
			for (int k = 0; k < base - 1; k++)
				cout << "    ";
		}
		cout << endl;
	}
	int num = pow(2, d_tot);

	cout << "floor0: " << source0 << "*0 ";
	for (int i = 0; i < num - 1; i++) {
		cout << "    ";
	}
	cout << source1 << "*" << num << endl;
	cout << "//////////////////////////////////////////////////" << endl;
	cout << f[4][0][3];

  for(int i=0;i<10;i++)
    for(int j=0;j<1025;j++)
        delete[] f[i][j];
}

