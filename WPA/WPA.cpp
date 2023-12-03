#include "WPA.h"
#include <cmath>
#include <algorithm>
#define PI 3.14
#define pi 3.14

WPA::WPA(const int wolf_num, const int dim, const double alpha, const double beta, const double w, const double S, const int max_iter):
	wolf_num(wolf_num),
	dim(dim),
	alpha(alpha),
	beta(beta),
	w(w),
	S(S),
	max_iter(max_iter)
{
	flag = false;
	besiege_flag = false;

	//̽����������ȡ[n��(��+1)��n����]֮���������
	int MIN_VALUE = wolf_num / (alpha + 1);
	int MAX_VALUE = wolf_num / alpha;
	exploring_wolf_num= rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
	//������������Ⱥ����(wolf_num)-̽������(exploring_wolf_num)-ͷ������(1)
	fierce_wolf_num = wolf_num - exploring_wolf_num - 1;

	//��ʼ����Ⱥ����
	wolfs.resize(wolf_num);
	MIN_VALUE = 0;
	MAX_VALUE = 100;
	for(auto &wolf:wolfs) {
		wolf.x = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
		wolf.y = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
		wolf.target_x = 500;
		wolf.target_y = 500;
	}
	//ͷ�ǳ�ʼ��
	update();

	//��ά�ռ䣬��ʾX��Y�����԰������С����Ϊ2
	StepA.resize(dim);
	StepB.resize(dim);
	StepC.resize(dim);
	//��ʼ��������Ϊ�Ĳ���
	//Χ������=1.024���ٻ�����=4.096�����߲���=2.048
	maxd = { 512,512 };
	mind = { -512,-512 };
	for (int i = 0; i < dim; ++i) {
		StepC[i] = abs(maxd[i] - mind[i]) / S;
		StepB[i] = 4 * StepC[i];
		StepA[i] = StepB[i] / 2;
	}

	draw();
}

void WPA::update()
{
	sort_wolfs_by_fitness();
	head_wolf_iter = wolfs.begin();
}

void WPA::sort_wolfs_by_fitness()
{
	//�ṹ������
	auto cmp=[](Wolf w1, Wolf w2)->bool{
		return w1.calculateFitness() < w2.calculateFitness() ? true : false;
	};
	//��Ⱥ�����Զ����������
	sort(wolfs.begin(), wolfs.end(),cmp);
}

double WPA::cal_distance(double x1,double y1,double x2,double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y1, 2));
}

void WPA::swap_wolf(vector<Wolf>::iterator first,vector<Wolf>::iterator second)
{
	//ǳ����
	Wolf tmp = *second;
	*second = *first;
	*first = tmp;
}



void WPA::wandering()
{
	for(int epoch=0;epoch< max_wandering_iter;++epoch) {
		for (int i = 1; i <= exploring_wolf_num; ++i) {
			if(wolfs[i].calculateFitness()<head_wolf_iter->calculateFitness()) {//��Yi����ͷ������֪��������ζŨ��Ylead������������̽��i����ԽϽ��Ҹ�̽�����п��ܲ�������
				update();//������ͷλ��
				flag = true;//Yi>Ylead
				draw();
				return;
			}else {//��Yi<Ylead����̽�����������ߣ���̽����������ֱ�ǰ��һ��
				double max_x = wolfs[i].x;
				double max_y = wolfs[i].y;
				for(int p=1;p<=h;++p) {
					double tempX = wolfs[i].x + sin(2 * pi*p / h)*StepA[0];
					double tempY = wolfs[i].y + sin(2 * pi*p / h)*StepA[1];
					if(cal_distance(max_x,max_y,wolfs[i].target_x,wolfs[i].target_y)> cal_distance(tempX, tempY, wolfs[i].target_x, wolfs[i].target_y)) {
						max_x = tempX;
						max_y = tempY;
					}
				}
				if (cal_distance(max_x, max_y, wolfs[i].target_x, wolfs[i].target_y) < cal_distance(wolfs[i].x, wolfs[i].y, wolfs[i].target_x, wolfs[i].target_y)) {
					wolfs[i].x = max_x;
					wolfs[i].y = max_y;
					cout << "������Ϊ��" << "��" << i + 1 << "̽�ǣ�" << "x=" << wolfs[i].x << ",y=" << wolfs[i].y << endl;
				}
				draw();
			}
		}
	}
	flag = true;//T>Tmax
}

void WPA::call()
{
	//�����ж�����d_near
	double d_near = 0;
	for (int i = 0; i < dim; ++i) {
		d_near += (1 / (dim*w))*abs(maxd[i] - mind[i]);
	}

	while(!besiege_flag) {

		//���ǿ���ͷ��
		for (int i = exploring_wolf_num + 1; i < wolf_num; ++i) {
			if (wolfs[i].calculateFitness() < head_wolf_iter->calculateFitness()) {
				swap_wolf(head_wolf_iter, head_wolf_iter + i);//����ͷ�Ǻ����ǵ����
				//draw();
				break;//����ִ���ٻ���Ϊ,Yi>Ylead
			}else {
				// wolfs[i].x = wolfs[i].x + StepB[0] * (head_wolf_iter->x - wolfs[i].x) / abs(head_wolf_iter->x - wolfs[i].x);
				// wolfs[i].y = wolfs[i].y + StepB[1] * (head_wolf_iter->y - wolfs[i].y) / abs(head_wolf_iter->y - wolfs[i].y);
				
				//���Ǳ�Ϯ
				if(head_wolf_iter->x-wolfs[i].x>1e-20) {
			 		wolfs[i].x = wolfs[i].x + StepB[0] * (head_wolf_iter->x - wolfs[i].x) / abs(head_wolf_iter->x - wolfs[i].x);
				}/*else {
					wolfs[i].x = wolfs[i].x + StepB[0] * 1;
				}*/
				if(head_wolf_iter->y-wolfs[i].y>1e-20) {
			 		wolfs[i].y = wolfs[i].y + StepB[1] * (head_wolf_iter->y - wolfs[i].y) / abs(head_wolf_iter->y - wolfs[i].y);
				}/*else {
					wolfs[i].y = wolfs[i].y + StepB[1] * 1;
				}*/
				cout << "�ٻ���Ϊ��" << "��" << i + 1 << "���ǣ�" << "x=" << wolfs[i].x << ",y=" << wolfs[i].y << endl;
				draw();

				//ת��Χ����Ϊ
				if (cal_distance(head_wolf_iter->x, head_wolf_iter->y, wolfs[i].x, wolfs[i].y) < d_near) {
					besiege_flag = true;
					update();
					//break;
					draw();
				}
			}
			//���Ǳ�Ϯ
			// if(head_wolf_iter->x-wolfs[i].x>1e-20) {
			// 	wolfs[i].x = wolfs[i].x + StepB[0] * (head_wolf_iter->x - wolfs[i].x) / abs(head_wolf_iter->x - wolfs[i].x);
			// }
			// if(head_wolf_iter->y-wolfs[i].y>1e-20) {
			// 	wolfs[i].y = wolfs[i].y + StepB[1] * (head_wolf_iter->y - wolfs[i].y) / abs(head_wolf_iter->y - wolfs[i].y);
			// }
			
		
		}
		

	}
	
}

void WPA::besiege()
{
	if (head_wolf_iter->calculateFitness() > 10) {
		cout << "Χ����Ϊ(ʧ��)" << endl;
		draw();
		return;
	}
	int MIN_VALUE = -1;
	int MAX_VALUE = 1;
	for (int i = 1; i < wolfs.size(); ++i) {
		int r = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
		double tempX = wolfs[i].x + r * StepC[0] * abs(wolfs[i].target_x - wolfs[i].x);
		double tempY = wolfs[i].y + r * StepC[1] * abs(wolfs[i].target_y - wolfs[i].y);
		if(cal_distance(tempX,tempY,wolfs[i].target_x,wolfs[i].target_y)<cal_distance(wolfs[i].x,wolfs[i].y,wolfs[i].target_x,wolfs[i].target_y)) {
			wolfs[i].x = tempX;
			wolfs[i].y = tempY;
		}
		cout << "Χ����Ϊ��" << "��" << i + 1 << "̽�ǻ����ǣ�" << "x=" << wolfs[i].x << ",y=" << wolfs[i].y << endl;
		draw();
	}
}

void WPA::run()
{
	auto start = steady_clock::now();
	for(int i=0;i<max_iter;++i) {
		wandering();
		if (flag) {
			call();
			if(besiege_flag) {
				besiege();
				update();
				if(head_wolf_iter->calculateFitness()<1.0) {
					break;
				}else {
					//�������ߣ�Ҫ�Ա�־����
					flag = false;
					besiege_flag = false;
				}
			}
		}
	}
	auto end = steady_clock::now();
	auto duration = duration_cast<seconds>(end - start);
	cout << "��������ʱ��Ϊ��" << duration.count() << "��" << endl;
	cout << "���Ž��Ϊ��" << "x=" << head_wolf_iter->x << "y=" << head_wolf_iter->y << endl;
	waitKey(0);
}

void WPA::draw()
{
	Mat img = Mat::zeros(600, 600,CV_8U);
	//����Ϊ��ɫ��ͷ��Ϊ��ɫ��̽��Ϊ��ɫ������Ϊ��ɫ
	int radius = 2;
	circle(img, Point(500, 500), radius, Scalar(0, 0, 255), -1);//������
	for(int i=1;i<=exploring_wolf_num;++i) {
		circle(img, Point(wolfs[i].x, wolfs[i].y), radius, Scalar(255, 255, 255), 1);//��̽��
	}
	circle(img, Point(head_wolf_iter->x, head_wolf_iter->y), radius, Scalar(0, 255, 0), -1);//��ͷ��
	for(int i=exploring_wolf_num+1;i<wolf_num;++i) {
		circle(img, Point(wolfs[i].x, wolfs[i].y), radius, Scalar(255, 0, 0), 1);//������
	}
	imshow("WPA", img);
	waitKey(0.001 * 1000);//�ȴ�0.001��
}

