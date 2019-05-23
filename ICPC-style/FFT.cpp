/*
Fast Fourier Transformation
Example Problem: UOJ 34 http://uoj.ac/problem/34)
Given two polynomials with degree n and m,
output their product
Example input:
1 2 #n and m
1 2 #polynomial 1, with degree 1. Given from lowest term to highest term. It should be 2x + 1
1 2 1 #polynomial 2, with degree m=2
Example output:
1 4 5 2 (2x + 1)(x^2 + 2x + 1) = (2x^3 + 5x^2 + 4x + 1)
*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <complex>
#include <algorithm>

using namespace std;

typedef complex<double> Complex;

const int maxn = 270000; //Slightly larger than the 2^18 = 262144, the smallest 2's power bigger than the maximum possible degree
const double pi = acos(-1.0);

//Using complex numbers to store the coefficients of the polynomial
Complex x[maxn],y[maxn];

//Op = 1 : DFT Op = -1 : IDFT
void fft(Complex p[],int n,int op)
{
	for (int i = 1,j = 0;i < n-1;++i)
	{
		for (int s = n;j ^= s >>= 1,~j&s;);
		if (i < j) swap(p[i],p[j]);
	}
	for (int d = 0;(1 << d) < n;++d)
	{
		int m = (1 << d);
		int m2 = m << 1;
		Complex unit(cos(pi/m*op),sin(pi/m*op));
		for (int i = 0;i < n;i += m2)
		{
			Complex unit_0 = 1;
			for (int j = 0;j < m;++j)
			{
				Complex & p1 = p[i+j+m];
				Complex & p2 = p[i+j];
				Complex t = p1 * unit_0;
				p1 = p2 - t;
				p2 = p2 + t;
				unit_0 *= unit;
			}
		}
	}
}

int main(int argc,char * argv[])
{
	int n,m;
	scanf("%d%d",&n,&m);
	for (int i = 0;i <= n;++i)
	{
		int temp;
		scanf("%d",&temp);
		x[i] = temp;
	}
	for (int i = 0;i <= m;++i)
	{
		int temp;
		scanf("%d",&temp);
		y[i] = temp;
	}
	int tar = n+m;
	//magic is the smallest 2's power greater than the sum of n+m, the degree of the resulting polynomial
	int magic = 0;
	while ((1 << magic) <= tar) magic++;
	fft(x,(1 << magic),1);
	fft(y,(1 << magic),1);
	for (int i = 0;i < (1 << magic);++i)
	x[i] = x[i] * y[i];
	fft(x,(1 << magic),-1);
	for (int i = 0;i <= tar;++i)
	{
		//Rounding the coefficients
		printf("%d ",(int)(x[i].real()/(1 << magic) + 0.499));
	}
	return 0;
}