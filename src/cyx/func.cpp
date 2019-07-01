
void getPos(int& x, int& y, int w, int m, int n, int z){//m 列数，n 行数 
	if(z <= m-1){
		y = 0;
		x = w*(z+1);
	}
	else if(z<=m+n-1){
		y = w*(m+1);
		x = w*(z-m+1);
	}
	else if(z<=2*m + n -1){
		x = w*(n+1);
		y = w*m - w*(z-n-m);
	}
	else if{z<=2*m + 2*n -1){
		y = 0;
		x = w*n - w*(z - 2*m - n);
	}
	else
		cout << "Wrong num"<<endl;
}
