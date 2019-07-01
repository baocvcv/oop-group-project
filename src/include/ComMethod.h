#ifndef COMMETHOD
#define COMMETHOD
#include <string>
using std::string;
using std::to_string;


class ComMethod {//�㷨���Ի���
public:
	virtual string load(int i) = 0;
	virtual ~ComMethod() {};
};


class Ours :public ComMethod {//ours�㷨���ԣ�costs=1��costb=0����С����ʼ��Һ��
public:
	string load(int i) {
		return "../testcase/ours/case" + to_string(i) + ".txt";
	};
	~Ours() {};
};

class Ourw :public ComMethod {//ourw�㷨���ԣ�costs=costb=0����С����Һ����
public:
	string load(int i) {
		return "../testcase/ourw/case" + to_string(i) + ".txt";
	};
	~Ourw() {};
};

#endif