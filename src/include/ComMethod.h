#ifndef COMMETHOD
#define COMMETHOD
#include <string>
using std::string;
using std::to_string;


class ComMethod {//算法策略基类
public:
	virtual string load(int i) = 0;
	virtual ~ComMethod() {};
};


class Ours :public ComMethod {//ours算法策略，costs=1，costb=0，最小化初始溶液数
public:
	string load(int i) {
		return "../testcase/ours/case" + to_string(i) + ".txt";
	};
	~Ours() {};
};

class Ourw :public ComMethod {//ourw算法策略，costs=costb=0，最小化溶液总数
public:
	string load(int i) {
		return "../testcase/ourw/case" + to_string(i) + ".txt";
	};
	~Ourw() {};
};

#endif