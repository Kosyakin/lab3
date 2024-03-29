
#include "pch.h"
#include<iostream>
#include<string>
#include<fstream>
#include<math.h>
#include<sstream>
const int ROW = 14;
const int COL = 130;
using namespace std;
void read_csv(double values[COL][ROW]) {
	ifstream file;
	double draft[2 * COL][ROW];
	file.open("24_Chita.csv");
	if (!file.is_open())
		cout << "Error" << endl;
	else {
		int i = 0;
		int j = 0;
		string line, value;
		getline(file, line);
		while (!file.eof()) {
			getline(file, line);
			stringstream s(line);
			while (getline(s, value, ',')) {
				stringstream ss(value);
				ss >> draft[i][j];
				j++;
			}
			i++;
		}
		for (int i = 0; i < 2 * COL; i += 2) {
			for (int j = 0; j < ROW; j++)
				values[i / 2][j] = draft[i][j];
		}
	}
	file.close();
}
int factor(int x) {
	if (x <= 1)
		return 1;
	else
		return x * factor(x - 1);
}
double differ(double i, double j, int month, int n, double values[COL][ROW]) {
	if (n == 0)
		return i - j;
	else if (n == 1)
		return (values[(int)i][month] - values[(int)j][month]);
	else
		return differ(differ(i + 1, j + 1, month, n - 1, values), differ(i, j, month, n - 1, values), month, 0, values);
}
double lagranj(double values[COL][ROW], int month, double x) {
	double sum = 0;
	double l;
	for (int i = 0; i < 12; i++) {
		l = 1;
		if (values[i][month] < 500) {
			for (int j = 0; j < 12; j++) {
				if (i != j) {
					l *= ((x - values[j][0]) / (values[i][0] - values[j][0]));
				}
			}
			sum += values[i][month] * l;
		}
	}
	return sum;
}
double newton_one(double values[COL][ROW], int month, double x) {
	double sum = values[0][month];
	double part;
	for (int i = 1; i < 6; i++) {
		part = 1;
		if (values[i][month] < 500) {
			for (int j = 0; j < i; j++)
				part *= (x - values[j][0]);
			part = part * differ(1, 0, month, i, values) / factor(i);
		}
		sum += part;
	}
	return sum;
}
double newton_two(double values[COL][ROW], int month, double x, int k) {
	double sum = values[5 + k][month];
	double part;
	for (int i = 1; i < 6; i++) {
		part = 1;
		if (values[i + k][month] < 500) {
			for (int j = 5 + k; j >= 5 + k - i + 1; j--)
				part *= (x - values[j][0]);
			part = part * differ(5 + k - i + 1, 5 + k - i, month, i, values) / factor(i);
		}
		sum += part;
	}
	return sum;
}
double stepen_polinom(int power, double*c, double x) {
	double px = 0;
	for (int i = 0; i <= power; i++)
		px += c[i] * pow(x, i);
	return px;
}
double *gauss(double **a, double *y, int n)
{
	double *x, max;
	int k, index;
	const double eps = 0.0001;
	x = new double[n];
	k = 0;
	while (k < n)
	{
		max = abs(a[k][k]);
		index = k;
		for (int i = k + 1; i < n; i++)
		{
			if (abs(a[i][k]) > max)
			{
				max = abs(a[i][k]);
				index = i;
			}
		}
		if (max < eps)
		{
			cout << "0 collumn ";
			cout << index << " matrix A" << endl;
			return 0;
		}
		for (int j = 0; j < n; j++)
		{
			double temp = a[k][j];
			a[k][j] = a[index][j];
			a[index][j] = temp;
		}
		double temp = y[k];
		y[k] = y[index];
		y[index] = temp;
		for (int i = k; i < n; i++)
		{
			double temp = a[i][k];
			if (abs(temp) < eps) continue;
			for (int j = 0; j < n; j++)
				a[i][j] = a[i][j] / temp;
			y[i] = y[i] / temp;
			if (i == k)  continue;
			for (int j = 0; j < n; j++)
				a[i][j] = a[i][j] - a[k][j];
			y[i] = y[i] - y[k];
		}
		k++;
	}
	for (k = n - 1; k >= 0; k--)
	{
		x[k] = y[k];
		for (int i = 0; i < k; i++)
			y[i] = y[i] - a[i][k] * x[k];
	}
	return x;
}
void graph(double values[COL][ROW], int month, double step) {
	//1
	ofstream graph1;
	graph1.open("graph.txt");
	for (double i = values[0][0]; i < values[0][0] + 12.0; i += step)
		graph1 << i << '\t' << lagranj(values, month, i) << endl;
	graph1.close();
	ofstream points1;
	points1.open("points.txt");
	for (int i = values[0][0]; i < values[0][0] + 12; i++)
		points1 << i << '\t' << values[i - (int)values[0][0]][month] << endl;
	points1.close();
	system("python lab3.py");
	//2
	ofstream graph2;
	graph2.open("graph.txt");
	for (double i = values[0][0]; i < values[0][0] + 6.0; i += step)
		graph2 << i << '\t' << newton_one(values, month, i) << endl;
	graph2.close();
	ofstream points2;
	points2.open("points.txt");
	for (int i = values[0][0]; i < values[0][0] + 6; i++)
		points2 << i << '\t' << values[i - (int)values[0][0]][month] << endl;
	points2.close();
	system("python lab3.py");
	//3
	ofstream graph3;
	graph3.open("graph.txt");
	int k = 5;
	for (double i = values[k][0]; i < values[k][0] + 6.0; i += step)
		graph3 << i << '\t' << newton_two(values, month, i, k) << endl;
	graph3.close();
	ofstream points3;
	points3.open("points.txt");
	for (int i = values[k][0]; i < values[k][0] + 6; i++)
		points3 << i << '\t' << values[i - (int)values[0][0]][month] << endl;
	points3.close();
	system("python lab3.py");
}
int main() {
	double values[COL][ROW];
	read_csv(values);
	double h = 0.1;
	graph(values, 8, h);
	double **a, *c, *y;
	int power;
	cout << "Enter power (max - 4): ";
	cin >> power;
	a = new double*[power];
	y = new double[power];
	for (int i = 0; i <= power; i++) {
		a[i] = new double[power];
		for (int j = 0; j <= power; j++) {
			a[i][j] = 0;
			for (int k = 0; k < COL; k++) {
				if (values[k][8] < 500)
					a[i][j] += pow(values[k][0], i + j);
			}
		}
	}
	y = new double[power];
	for (int i = 0; i <= power; i++)
	{
		y[i] = 0;
		for (int k = 0; k < COL; k++) {
			if (values[k][8] < 500)
				y[i] += values[k][8] * pow(values[k][0], i);
		}

	}
	c = gauss(a, y, power);
	//4
	ofstream graph4;
	graph4.open("graph.txt");
	for (double i = values[0][0]; i < values[0][0] + COL; i += h)
		graph4 << i << '\t' << stepen_polinom(power, c, i) << endl;
	graph4.close();
	ofstream points4;
	points4.open("points.txt");
	for (int i = values[0][0]; i < values[0][0] + COL; i++)
		if (values[i - (int)values[0][0]][8] < 500)
			points4 << i << '\t' << values[i - (int)values[0][0]][8] << endl;
	points4.close();
	system("python lab3.py");

	delete[] a;
	delete[] y;
	delete[] c;

	return 0;
}