#pragma once
#include <cmath>
class Wolf
{
public:
	Wolf();
	Wolf(double x, double y);
	double x;//��λ��x���� 
	double y;//��λ��y����
	double target_x;//Ŀ��λ��x����
	double target_y;//Ŀ��λ��y����
	//double fitness;//��Ӧ�� 

	//������Ӧ�� 
	double calculateFitness();
};
