#include"include/Graph.h"
#include<cmath>
#include<fstream>
#include<cstring>
#include<iostream>

using namespace std;

//#define f(a,b,c) model.getVarByName(f((a),(b),(c)))

void Graph::compute() {
	try {
		Constrain();

		model.setObjective(f(-1, 0, 1)*costs + f(-1, 0, 0)*costb, GRB_MINIMIZE);
		model.optimize();

		Record();
		Remove();
	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}
}

bool Graph::load(string name) {
	fstream in;
	memset(used, 0, sizeof(used));
	TCS.clear();
	in.open(name);
	if (!in.is_open()) {
		cout << "File Open Failed\n";
		return 0;
	}

	string temp;
	pair<int, int> a;
	int num;
	in >> temp >> costs;
	in >> temp >> costb;
	if (costb == 0)
		costb = 0.001;
	in >> temp >> d_tot;
	in >> temp >> num;
	TCS.assign(num, a);
	in >> temp;
	for (int i = 0; i < num; i++) {
		in >> TCS[i].first;
		used[TCS[i].first] = i + 1;
	}
	in >> temp;
	for (int i = 0; i < num; i++)
		in >> TCS[i].second;
	in.close();
	return 1;
}

bool Graph::load(int i) {
	fstream in;
	string file = method->load(i);
	memset(used, 0, sizeof(used));
	TCS.clear();
	in.open(file);
	if (!in.is_open()) {
		cout << "File Open Failed\n";
		return 0;
	}

	string temp;
	pair<int, int> a;
	int num;
	in >> temp >> costs;
	in >> temp >> costb;
	if (costb == 0)
		costb = 0.001;
	in >> temp >> d_tot;
	in >> temp >> num;
	TCS.assign(num, a);
	in >> temp;
	for (int i = 0; i < num; i++) {
		in >> TCS[i].first;
		used[TCS[i].first] = i + 1;
	}
	in >> temp;
	for (int i = 0; i < num; i++)
		in >> TCS[i].second;
	in.close();

	return 1;
}

void Graph::Init(int d_tot) {
	GRBVar source0,source1;
	string name;
	Result a;
	model.set(GRB_DoubleParam_TimeLimit, 1800.0);

	result.assign(pow2(d_tot) + 1, a);

	source0 = model.addVar(0, GRB_INFINITY, 0.0, GRB_INTEGER, "source0");
	source1 = model.addVar(0, GRB_INFINITY, 0.0, GRB_INTEGER, "source1");
  
  //addVar
	for (int i = 0; i < d_tot; i++) {
		int point = pow2(i) + 1;
    for(int j=0;j<point;j++)
      for(int k=j;k<j+point;k++){
        name = "floor"+to_string(i)+"_"+to_string(j)+"_"+to_string(k);
        model.addVar(0,GRB_INFINITY,0,GRB_INTEGER,name);
      }
	}
	for (int i = 0; i <= pow2(d_tot); i++) {
		name = "waste_" + to_string(i);
		model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, name);
	}

	model.update();

  //addConstr
	GRBLinExpr netflow, equ;
	netflow = 0;
	netflow = source0 - f(0, 0, 0) - f(0, 0, 1);
	model.addConstr(netflow == 0);
	netflow = 0;
	netflow = source1 - f(0, 1, 1) - f(0, 1, 2);
	model.addConstr(netflow == 0);

	for (int i = 0; i < d_tot - 1; i++) {
		int point1 = pow2(i) + 1;
		int point2 = pow2(i + 1) + 1;

		for (int j = 0; j < point1; j++) {
			equ = 0;
      //symmetrical constrains
			for (int k = 2 * j + 1; k < j + point1; k++) {
				int temp = k - j;
				equ = f(i, j, k) - f(i, temp, k);
				model.addConstr(equ == 0);
			}
		}

    //flow constrains
		for (int j = 0; j < point2; j++) {
			netflow = 0;
			if (j < point1) {

				for (int k = 0; k <= j; k++) {
					netflow += f(i, k, j);
				}
				for (int k = j; k < j + point2; k++) {
					netflow -= f(i + 1, j, k);
				}
				model.addConstr(netflow == 0);
			}
			else {
				for (int k = point1 - 1; k >= j - point1 + 1; k--) {
					netflow += f(i, k, j);
				}
				for (int k = j; k < j + point2; k++) {
					netflow -= f(i + 1, j, k);
				}
				model.addConstr(netflow == 0);
			}
		}
	}

  //symmetrical constrain for top floor
	int point1 = pow2(d_tot - 1) + 1;
	for (int i = 0; i < point1; i++) {
		for (int k = 2 * i + 1; k < i + point1; k++) {
			int temp = k - i;
			equ = f(d_tot - 1, i, k) - f(d_tot - 1, temp, k);
			model.addConstr(equ == 0);
		}
	}

	model.update();
}

//Update Constrains
void Graph::Constrain() {
	GRBLinExpr netflow, equ;
	int point1 = pow2(d_tot - 1) + 1;
	int point2 = pow2(d_tot) + 1;

	//flow constrains for top floor
	for (int i = 0; i < point2; i++) {
		netflow = 0;
		if (used[i]) {
			if (i < point1) {
				for (int j = 0; j <= i; j++) {
					netflow += f(d_tot - 1, j, i);
				}
			}
			else {
				for (int j = point1 - 1; j >= i - point1 + 1; j--) {
					netflow += f(d_tot - 1, j, i);
				}
			}
			netflow -= f(d_tot, i, 0);

			model.addConstr(netflow >= TCS[used[i] - 1].second, "remove" + to_string(i));
		}
		else if(i<point1){
			netflow = f(d_tot - 1, 0, i);
			model.addConstr(netflow == 0, "bufferlimit" + to_string(i));
		}
	}
	
	model.update();
}

//record results
void Graph::Record(){
	GRBVar source1, source0;
	source1 = model.getVarByName("source1");
	source0 = model.getVarByName("source0");
	result.push_back(Result());
	result[count].Ns = (int)source1.get(GRB_DoubleAttr_X);
	result[count].Nb = (int)source0.get(GRB_DoubleAttr_X);
	result[count].Nw = result[count].Nb + result[count].Ns;
	for (int i = 0; i < (int)TCS.size(); i++) {
		result[count].Nw = result[count].Nw - TCS[i].second;
	}
	result[count].Nd = 0;
	for (int i = 0; i < d_tot; i++)
		for (int j = 0; j <= pow2(i); j++)
			for (int k = j; k <= j + pow2(i); k++) {
				if (k == 2 * j)
					continue;
				if ((int)f(i, j, k).get(GRB_DoubleAttr_X) > 1e-6)
					result[count].Nd++;
			}
	result[count].Nd = result[count].Nd / 2;
  count++;
}

void Graph::Remove() {
	GRBConstr temp;
	string name;
	int num = pow2(d_tot);
	for (int i = 0; i < num; i++) {
		if (used[i]) {
			name = "remove" + to_string(i);
			temp = model.getConstrByName(name);
			model.remove(temp);
		}
		else if(i<pow2(d_tot-1)+1){
			name = "bufferlimit" + to_string(i);
			temp = model.getConstrByName(name);
			model.remove(temp);
		}
	}
}

bool Graph::check() {
	cout << endl;
    int source1 = (int)model.getVarByName("source1").get(GRB_DoubleAttr_X);
    int source0 = (int)model.getVarByName("source0").get(GRB_DoubleAttr_X);
	int f[1024][1024] = { 0 };

	int point1 = pow(2, d_tot - 1) + 1;
	int point2 = pow(2, d_tot) + 1;
	string name;
	for (int j = 0; j <= point2; j++) {
		if (j < point1) {
			for (int k = 0; k <= j; k++) {
				name = "floor" + to_string(d_tot - 1) + "_" +
					to_string(k) + "_" + to_string(j);
				f[k][j] = (int)model.getVarByName(name).get(GRB_DoubleAttr_X);
			}
		}
		else {
			for (int k = point1 - 1; k >= j - point1 + 1; k--) {
				name = "floor" + to_string(d_tot - 1) + "_" +
					to_string(k) + "_" + to_string(j);
				f[k][j] = (int)model.getVarByName(name).get(GRB_DoubleAttr_X);
			}
		}
	}
	int node[1024];
	memset(node, 0, sizeof(node));
	for (int j = 0; j < point1; j++)
		for (int k = 0; k < point2; k++) {
			node[k] += f[j][k];
		}
	for (int i = 0; i < (int)TCS.size(); i++) {
		int num = TCS[i].first;
		if (node[num] < TCS[i].second) {
			cout << "False Result!" << endl;
			return 0;
		}
	}
	cout << "Correct Result" << endl;
	return 1;
}

void Graph::translator() {
	fstream out("input.txt", ios::out | ios::trunc);
	if (!out.is_open()) {
		cout << "Open Failed" << endl;
		return;
    }
    int ***f;//f[10][1025][1025];
    f = new int**[10];
    for(int i = 0; i < 10; i++){
        f[i] = new int*[1025];
        for(int j = 0; j < 1025; j++){
            f[i][j] = new int[1025];
        }
    }
    int maxc = pow2(d_tot);

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 1025; j++)
			for (int k = 0; k < 1025; k++)
				f[i][j][k] = 0;
  int source0 = (int)model.getVarByName("source0").get(GRB_DoubleAttr_X);
  int source1 = (int)model.getVarByName("source1").get(GRB_DoubleAttr_X);

	for (int i = 0; i < d_tot; i++) {
		int point1 = pow(2, i) + 1;
		int point2 = pow(2, i + 1) + 1;
		string name;

		for (int j = 0; j < point2; j++) {
			if (j < point1) {
				for (int k = 0; k <= j; k++) {
					name = "floor" + to_string(i) + "_" +
						to_string(k) + "_" + to_string(j);
					f[i][k][j] = (int)model.getVarByName(name).get(GRB_DoubleAttr_X);
				}
			}
			else {
				for (int k = point1 - 1; k >= j - point1 + 1; k--) {
					name = "floor" + to_string(i) + "_" +
						to_string(k) + "_" + to_string(j);
					f[i][k][j] = (int)model.getVarByName(name).get(GRB_DoubleAttr_X);
				}
			}
		}
	}

  int dispense = 1;
	int count = source0+source1+1;
	vector<int*>memory[2015], dis0, dis1, mix, edge;

	for (int i = 1; i <= d_tot; i++) {
		int point1 = pow2(i - 1) + 1;
		int point2 = pow2(i) + 1;
		int step = pow2(d_tot - i);
		for (int j = 0; j < point2; j++) {
			int c = step * j;
			if (j < point1) {
				for (int k = 0; k * 2 * step < c; k++) {
					if (f[i - 1][k][j]) {
						int kc1 = k * 2 * step;
						int kc2 = 2 * c - kc1;
						for (int m = 0; m < f[i - 1][k][j]; m++) {
							int tempkc1 = -1;
							int tempkc2 = -1;
							int tempc = count;
							mix.push_back(new int[2]{ count,c });
							memory[c].push_back(new int[2]{ count,2 });
							count++;
							if (kc1 == 0) {
								tempkc1 = dispense;
								dis0.push_back(new int[2]{ tempkc1,0 });
                dispense++;
							}
							else {
								for (int n = 0; n < (int)memory[kc1].size(); n++)
									if (memory[kc1][n][1]) {
										tempkc1 = memory[kc1][n][0];
										memory[kc1][n][1]--;
										break;
									}
								if (tempkc1 == -1) {
									cout << "Wrong Answer! in kc1" << endl;
									system("pause");
									return;
								}
							}

							if (kc2 == maxc) {
								tempkc2 = dispense;
								dis1.push_back(new int[2]{ tempkc2,maxc });
                dispense++;
							}
							else {
								for (int n = 0; n < (int)memory[kc2].size(); n++)
									if (memory[kc2][n][1]) {
										tempkc2 = memory[kc2][n][0];
										memory[kc2][n][1]--;
										break;
									}
								if (tempkc2 == -1) {
									cout << "Wrong Answer! in kc2" << endl;
									system("pause");
									return;
								}
							}
							edge.push_back(new int[2]{ tempkc1,tempc });
							edge.push_back(new int[2]{ tempkc2,tempc });
						}
					}
				}
			}
			else {
				for (int k = point1 - 1; k * 2 * step > c; k--) {
					if (f[i - 1][k][j]) {
						int kc2 = k * 2 * step;
						int kc1 = 2 * c - kc2;
						for (int m = 0; m < f[i - 1][k][j]; m++) {
							int tempkc1 = -1;
							int tempkc2 = -1;
							int tempc = count;
							mix.push_back(new int[2]{ count,c });
							memory[c].push_back(new int[2]{ count,2 });
							count++;
							if (kc1 == 0) {
								tempkc1 = dispense;
								dis0.push_back(new int[2]{ tempkc1,0 });
                dispense++;
							}
							else {
								for (int n = 0; n < (int)memory[kc1].size(); n++)
									if (memory[kc1][n][1]) {
										tempkc1 = memory[kc1][n][0];
										memory[kc1][n][1]--;
										break;
									}
								if (tempkc1 == -1) {
									cout << "Wrong Answer! in kc1" << endl;
									system("pause");
									return;
								}
							}

							if (kc2 == maxc) {
								tempkc2 = dispense;
								dis1.push_back(new int[2]{ dispense,maxc });
                dispense++;
							}
							else {
								for (int n = 0; n < (int)memory[kc2].size(); n++)
									if (memory[kc2][n][1]) {
										tempkc2 = memory[kc2][n][0];
										memory[kc2][n][1]--;
										break;
									}
								if (tempkc2 == -1) {
									cout << "Wrong Answer! in kc2" << endl;
									system("pause");
									return;
								}
							}
							edge.push_back(new int[2]{ tempkc1,tempc });
							edge.push_back(new int[2]{ tempkc2,tempc });
						}
					}
				}
			}
		}
	}

  //const int detect = count;
  //count++;

  const int waste = count;
  count++;

	for (int i = 0; i < (int)TCS.size(); i++) {
		int c = TCS[i].first;
		int num = TCS[i].second;
		for (int j = 0; j < num; j++) {
			int temp1 = -1;
			for (int k = 0; k < (int)memory[c].size(); k+=2)
				if (memory[c][k][1]>0) {
					temp1 = memory[c][k][0];
					memory[c][k][1]=0;
          edge.push_back(new int[2]{temp1,waste});
					break;
				}
			//edge.push_back(new int[2]{ temp1, detect });
		}
	}


	for (int i = 0; i <= maxc; i++) {
		if ((int)memory[i].size() == 0)
			continue;
		for (int j = 0; j < memory[i].size(); j++) {
			if (memory[i][j][1] == 0)
				continue;
			for (int k = 0; k < memory[i][j][1]; k++) {
				edge.push_back(new int[2]{ memory[i][j][0], waste });
			}
		}
	}

    out << "DAGNAME (GRAPH" << d_tot << ")" << endl;
    for (int i = 0; i < (int)dis0.size(); i++)
		out << "NODE (" << dis0[i][0] << ", DISPENSE, 0, " << dis0[i][1] << ", DIS0)" << endl;

	for (int i = 0; i < (int)dis1.size(); i++)
		out << "NODE (" << dis1[i][0] << ", DISPENSE, 1, " << dis1[i][1] << ", DIS1)" << endl;


	for (int i = 0; i < (int)mix.size(); i++)
        out << "NODE (" << mix[i][0] << ", MIX, " << mix[i][1] << ", 1, MIX" << i << ")" << endl;

	//out << "NODE (" << detect << ", OUTPUT, output, OUT1)" << endl;

	out << "NODE (" << waste << ", OUTPUT, output, OUT1)" << endl;

    for (int i = 0; i < (int)edge.size(); i++){
        out << "EDGE (" << edge[i][0] << ", " << edge[i][1]<<", ";
        int temp = -1;
        for(int j=0;j<dis0.size();j++)
            if(dis0[j][0] == edge[i][0]){
                temp = 0;
                break;
            }
        if(temp==-1)
            for(int j=0;j<dis1.size();j++)
                if(dis1[j][0] == edge[i][0]){
                    temp = 1;
                    break;
                }
        if(temp==-1)
            for(int j=0;j<mix.size();j++)
                if(mix[j][0] == edge[i][0]){
                    temp = mix[j][1];
                    break;
                }
        out << temp << ")" << endl;
    }

	out << "TIME (9999)" << endl;
	out << "SIZE (100, 100)" << endl;
	out << "MOD (DIS0, " << (int)dis0.size()/5+1 << " )" << endl;
	out << "MOD (DIS1, " << (int)dis0.size()/5+1 << " )" << endl;
	for (int i = 0; i < (int)mix.size(); i++) {
		out << "MOD (MIX" << i << ", 2, 3)" << endl;
	}

    out << "MOD (OUT1, " << (dis0.size() + dis1.size()) / 10 + 1 << ")" << endl;

    out.close();
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 1025; j++){
            delete []f[i][j];
        }
        delete []f[i];
    }
    delete []f;

	return;
}
