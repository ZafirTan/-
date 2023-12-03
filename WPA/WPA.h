#pragma once
#include "Wolf.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
using namespace std::chrono;


class WPA
{
public:
	WPA(const int wolf_num = 50, const int dim = 2, const double alpha = 4, const double beta = 6, const double w = 500, const double S = 1000, const int max_iter = 1000);
	//h������
	void wandering();//����
	void call();//�ٻ�
	void besiege();//Χ��
	void run();//����
	void update();//����ͷ��λ��
	void sort_wolfs_by_fitness();//��������
	double cal_distance(double x1, double y1, double x2, double y2);//�������
	void swap_wolf(vector<Wolf>::iterator first,vector<Wolf>::iterator second);//��������

	void draw();//��ͼ
private:

	int h = 10;//h������
	double alpha = 4;//̽�Ǳ�������
	double beta = 6;//��Ⱥ���±�������
	double w = 500;//�����ж�����
	int wolf_num = 50;//��Ⱥ����
	int exploring_wolf_num;//̽����������ȡ[n��(��+1)��n����]֮���������
	int fierce_wolf_num;//��������
	vector<Wolf> wolfs;//��Ⱥ����
	vector<Wolf>::iterator head_wolf_iter;//ͷ��ָ��
	vector<int> besiege_wolfs;//Χ������

	vector<int> maxd;//ά�����ֵ
	vector<int> mind;//ά����Сֵ
	vector<double> StepA;//���߲���
	vector<double> StepB;//�ٻ�����
	vector<double> StepC;//Χ������
	double S = 1000;//��������
	int dim = 2;//ά��

	int max_wandering_iter = 30;//������ߴ���
	int max_iter = 1000;//����������

	bool flag=false;//ͷ�Ǹ��±�־	
	bool besiege_flag = false;//Χ����Ϊ��־

};