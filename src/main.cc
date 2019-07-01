#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#include"Graph.h"
#include"ComMethod.h"
#include"GraphCreator.h"

using namespace std;

void print(Result a) {//´òÓ¡½á¹û
	cout << "N: " << a.N << endl;
	cout << "Ns: " << a.Ns<<endl;
	cout << "Nb: " << a.Nb<<endl;
	cout << "Nw: " << a.Nw<<endl;
	cout << "Nd: " << a.Nd<<endl;
}

int main(int argc, char* argv[]) {
	Graph* graph;
	GraphCreator creat;
	GRBEnv env = GRBEnv();
	Result result1, result2;
	graph = creat.CreatGraph(env);

	int N;
	string name;
	name = argv[1];
	if (!graph->load(name))
		return 0;
	N = graph->get_d();
	graph->Init(N);
	graph->compute();
	Result result = graph->save();
	graph->translator();
	return 0;
}
