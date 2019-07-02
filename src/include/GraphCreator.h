#ifndef GRAPHCREATOR
#define GRAPHCREATOR

#include"gurobi_c++.h"
#include"Graph.h"

class GraphCreator {//工厂模式
public:
	virtual Graph* CreatGraph(GRBEnv env) {
		GRBModel model = GRBModel(env);
		Graph* graph = new Graph(model);
		return graph;
	}
};


#endif // !GRAPHCREATOR
