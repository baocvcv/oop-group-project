
#ifndef SHOW
#define SHOW
#include<vector>
#include"gurobi_c++.h"
using std::vector;

struct Result {
  double  Ns=0, Nb=0, Nw=0, Nd=0;
  int N=0;
};


class Show{
public:
	virtual Result operator()(const int d_tot, const vector<Result>result) { return Result(); }
	virtual void operator()(const int d_tot,GRBModel& model) {}
	virtual ~Show() {};
};

class GetFlow :public Show {
public:
	void operator()(const int d_tot,GRBModel& model);
};

class Print:public Show{
public:
  Result operator()(const int d_tot, const vector<Result>result);
};

class Save:public Show{
public:
  Result operator()(const int d_tot, const vector<Result>result);
};
#endif

