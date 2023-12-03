#include <iostream>
#include <map>
#include <cmath>
#include <chrono>
#include <iterator>
#include <opencv2/opencv.hpp>
using namespace std;

#define PI 3.14
#define pi 3.14



class Point
{
public:
	Point(double x,double y)
	{
		m_X = x;
		m_Y = y;
	}
	Point()
	{
		
	}
	double m_X;
	double m_Y;
	// Point operator=(const Point &point)const
	// {
	// 	return Point(point.m_X, point.m_Y);
	// }
};

class Wolf
{
public:
	Wolf()
	{
		
	}
	Wolf(Point position)
	{
		m_Position = position;
	}
	Wolf(double x,double y)
	{
		m_Position = Point(x, y);
	}


	Point m_Position;//��ǰ�ǵ�����λ��
	int m_index;//��ǰ�ǵı��
	double getX()
	{
		return m_Position.m_X;
	}
	double getY()
	{
		return m_Position.m_Y;
	}
	void setX(const double x)
	{
		m_Position.m_X = x;
	}
	void setY(const double y)
	{
		m_Position.m_Y = y;
	}
	void setPos(Point p)
	{
		m_Position = p;
	}

private:

	// Wolf operator=(const Wolf &w)const
	// {
	// 	Wolf temp;
	// 	temp.m_Position = w.m_Position;
	// 	temp.m_index = w.m_index;
	// 	return temp;
	// }
};

class WPA
{
public:
	WPA(const int wolf_num = 50, const int dim = 2, const double alpha = 4, const double beta = 6, const double w = 500, const double S = 1000, const int max_iter = 1000, const Point target_pos = Point(500, 500)) :
		m_WolfNum(wolf_num),
		dim(dim),
		m_alpha(alpha),
		m_beta(beta),
		m_w(w),
		S(S),
		max_iter(max_iter),
		m_TargetPos(target_pos)
	{
		flag = false;
		besiege_flag = false;

		//̽����������ȡ[n��(��+1)��n����]֮���������
		int MIN_VALUE = wolf_num / (alpha + 1);
		int MAX_VALUE = wolf_num / alpha;
		m_ExploringWolfNum = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
		//������������Ⱥ����(wolf_num)-̽������(exploring_wolf_num)-ͷ������(1)
		m_FierceWolfNum = wolf_num - m_ExploringWolfNum - 1;

		//��ʼ����Ⱥ����
		m_HeadWolfs.resize(1);//ͷ�������ʼ��
		m_ExploringWolfs.resize(m_ExploringWolfNum);//̽�������ʼ��
		m_FierceWolfs.resize(m_FierceWolfNum);//���������ʼ��

		vector<Wolf> temp(m_WolfNum);//��ʱ��Ⱥ����
		MIN_VALUE = 0;
		MAX_VALUE = 100;
		for(int i=0;i<temp.size();++i) {
			//��Ⱥ�ĳ�ʼλ����x=[0,100],y=[0,100]֮��������
			temp[i].m_Position.m_X = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
			temp[i].m_Position.m_Y = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
			temp[i].m_index = i + 1;
		}
		sortWolfsByFitness(temp);//������Ӧ������
		for (int i = 0; i < m_headWolfNum; ++i) {//ͷ��
			m_HeadWolfs[i] = temp[i];
		}
		m_HeadWolfIter = m_HeadWolfs.begin();
		for(int i=0;i<m_ExploringWolfNum;++i) {//����
			m_ExploringWolfs[i] = temp[m_headWolfNum + i];
		}
		for(int i=0;i<m_FierceWolfNum;++i) {//����
			m_FierceWolfs[i] = temp[m_ExploringWolfNum + i];
		}


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

		drawWofls();
	}


	/*
	 * ������: calcFitness
	 * ����: ������Ӧ�ȣ�������Ҫ����Ŀ�꺯����fitness<=0
	 * ����:
	 *   - X1: ԭ���X����
	 *   - Y1: ԭ���Y����
	 *   - X2: Ŀ����X����
	 *   - Y2: Ŀ����Y����
	 * ����ֵ: �����ľ���
	 */
	double calcFitness(const int X1, const int Y1, const int X2, const int Y2)
	{
		//�������빫ʽ
		double d = sqrt(pow(X1 - X2, 2) + pow(Y1 - Y2, 2));
		return -d;//����ԽС����Ӧ��Խ��
	}
	double calcFitness(Point p1,Point p2)
	{
		//�������빫ʽ
		double d = sqrt(pow(p1.m_X - p2.m_X, 2) + pow(p1.m_Y - p2.m_Y, 2));
		return -d;//����ԽС����Ӧ��Խ��
	}

	/*
	 * ������: sortWolfsByFitness
	 * ����: ��Ӧ��������
	 * ����:��
	 * ����ֵ: ��
	 */
	void sortWolfsByFitness(vector<Wolf> &wolfs)
	{
		//�ṹ������
		auto cmp = [&](Wolf &w1, Wolf &w2)->bool {
			return calcFitness(w1.m_Position,m_TargetPos) > calcFitness(w2.m_Position,m_TargetPos) ? true : false;
		};
		//��Ⱥ�����Զ����������
		sort(wolfs.begin(), wolfs.end(), cmp);
	}

	void update()
	{
		//����������Ⱥ����
		vector<Wolf> temp;
		//�ϲ�����
		for (int i = 0; i < m_headWolfNum; ++i) {//ͷ��
			temp.push_back(m_HeadWolfs[i]);
		}
		m_HeadWolfIter = m_HeadWolfs.begin();
		for (int i = 0; i < m_ExploringWolfNum; ++i) {//����
			temp.push_back(m_ExploringWolfs[i]);
		}
		for (int i = 0; i < m_FierceWolfNum; ++i) {//����
			temp.push_back(m_FierceWolfs[i]);
		}
		sortWolfsByFitness(temp);//������Ӧ������
		for (int i = 0; i < m_headWolfNum; ++i) {//ͷ��
			m_HeadWolfs[i] = temp[i];
		}
		m_HeadWolfIter = m_HeadWolfs.begin();
		for (int i = 0; i < m_ExploringWolfNum; ++i) {//����
			m_ExploringWolfs[i] = temp[m_headWolfNum + i];
		}
		for (int i = 0; i < m_FierceWolfNum; ++i) {//����
			m_FierceWolfs[i] = temp[m_ExploringWolfNum + i];
		}
	}


	/*
	 * ������: wolfsWandering
	 * ����: WPA�㷨�е����ߺ���
	 * ����:��
	 * ����ֵ: ��
	 */
	void wolfsWandering()
	{
		for (int epoch = 0; epoch < max_wandering_iter; ++epoch) {
			for (int i = 0; i < m_ExploringWolfNum; ++i) {
				if (calcFitness(m_ExploringWolfs[i].m_Position,m_TargetPos) > calcFitness(m_HeadWolfIter->m_Position,m_TargetPos)) {//��Yi����ͷ������֪��������ζŨ��Ylead������������̽��i����ԽϽ��Ҹ�̽�����п��ܲ�������
					//������ͷλ��
					Wolf temp = m_ExploringWolfs[i];
					m_ExploringWolfs[i] = *m_HeadWolfIter;
					*m_HeadWolfIter = temp;
					flag = true;//Yi>Ylead
					drawWofls();
					return;
				}
				else {//��Yi<Ylead����̽�����������ߣ���̽����������ֱ�ǰ��һ��
					double max_x = m_ExploringWolfs[i].m_Position.m_X;
					double max_y = m_ExploringWolfs[i].m_Position.m_Y;
					for (int p = 1; p <= m_h; ++p) {
						double tempX= m_ExploringWolfs[i].m_Position.m_X + sin(2 * pi*p / m_h)*StepA[0];
						double tempY= m_ExploringWolfs[i].m_Position.m_Y + sin(2 * pi*p / m_h)*StepA[1];
						if(calcFitness(Point(max_x,max_y),m_TargetPos)<calcFitness(Point(tempX,tempY),m_TargetPos)) {
							max_x = tempX;
							max_y = tempY;
						}
					}
					m_ExploringWolfs[i].m_Position = Point(max_x, max_y);
					cout << "������Ϊ��" << "���Ϊ" << m_ExploringWolfs[i].m_index << "��̽�ǣ�" << "x=" << m_ExploringWolfs[i].getX() << ",y=" << m_ExploringWolfs[i].getY() << endl;
					drawWofls();
				}
			}
		}
		flag = true;//T>Tmax
	}

	/*
	 * ������: wolfsCall
	 * ����: WPA�㷨�е��ٻ�����
	 * ����:��
	 * ����ֵ: ��
	 */
	void wolfsCall()
	{

		//�����ж�����d_near
		double d_near = 0;//d_near=2.048
		for (int i = 0; i < dim; ++i) {
			d_near += (1 / (dim*m_w))*abs(maxd[i] - mind[i]);
		}

		while (!besiege_flag) {

			//���ǿ���ͷ��
			for (int i = 0; i < m_FierceWolfNum; ++i) {
				if (calcFitness(m_FierceWolfs[i].m_Position,m_TargetPos) > calcFitness(m_HeadWolfIter->m_Position,m_TargetPos)) {//Yi>Ylead
					//����ͷ�Ǻ����ǵ���ݣ�Yi>Ylead
					Wolf temp = m_FierceWolfs[i];
					m_FierceWolfs[i] = *m_HeadWolfIter;
					*m_HeadWolfIter = temp;
					//drawWofls();
					break;//����ִ���ٻ���Ϊ
				}else {//Yi<Ylead

					//���Ǳ�Ϯ
					// m_FierceWolfs[i].m_Position.m_X = m_FierceWolfs[i].getX() + StepB[0] * (m_HeadWolfIter->getX() - m_FierceWolfs[i].getX()) / abs(m_HeadWolfIter->getX() - m_FierceWolfs[i].getX());
					// m_FierceWolfs[i].m_Position.m_Y = m_FierceWolfs[i].getY() + StepB[1] * (m_HeadWolfIter->getY() - m_FierceWolfs[i].getY()) / abs(m_HeadWolfIter->getY() - m_FierceWolfs[i].getY());
					//���Ǳ�Ϯ
					if(m_HeadWolfIter->getX()-m_FierceWolfs[i].getX()>1e-20) {
						m_FierceWolfs[i].m_Position.m_X = m_FierceWolfs[i].getX() + StepB[0] * (m_HeadWolfIter->getX() - m_FierceWolfs[i].getX()) / abs(m_HeadWolfIter->getX() - m_FierceWolfs[i].getX());
					}/*else {
						m_FierceWolfs[i].m_Position.m_X = m_FierceWolfs[i].getX() + StepB[0] * 1;
					}*/
					if(m_HeadWolfIter->getY()-m_FierceWolfs[i].getY()>1e-20) {
						m_FierceWolfs[i].m_Position.m_Y = m_FierceWolfs[i].getY() + StepB[1] * (m_HeadWolfIter->getY() - m_FierceWolfs[i].getY()) / abs(m_HeadWolfIter->getY() - m_FierceWolfs[i].getY());
					}/*else {
						m_FierceWolfs[i].m_Position.m_Y = m_FierceWolfs[i].getY() + StepB[1] * 1;
					}*/
					
					cout << "�ٻ���Ϊ��" << "���Ϊ" << m_FierceWolfs[i].m_index << "�����ǣ�" << "x=" << m_FierceWolfs[i].getX() << ",y=" << m_FierceWolfs[i].getY() << endl;
					drawWofls();

					//ת��Χ����Ϊ
					if (abs(calcFitness(m_HeadWolfIter->m_Position, m_FierceWolfs[i].m_Position)) < d_near) {
						//m_BesiegeWolfs.push_back(m_FierceWolfs[i]);//����Χ������
						besiege_flag = true;
						drawWofls();
						//break;
					}
				}
				
			
				//���Ǳ�Ϯ
				// if(m_HeadWolfIter->getX()-m_FierceWolfs[i].getX()>1e-20) {
				// 	m_FierceWolfs[i].m_Position.m_X = m_FierceWolfs[i].getX() + StepB[0] * (m_HeadWolfIter->getX() - m_FierceWolfs[i].getX()) / abs(m_HeadWolfIter->getX() - m_FierceWolfs[i].getX());
				// }else {
				// 	m_FierceWolfs[i].m_Position.m_X = m_FierceWolfs[i].getX() + StepB[0] * 1;
				// }
				// if(m_HeadWolfIter->getY()-m_FierceWolfs[i].getY()>1e-20) {
				// 	m_FierceWolfs[i].m_Position.m_Y = m_FierceWolfs[i].getY() + StepB[1] * (m_HeadWolfIter->getY() - m_FierceWolfs[i].getY()) / abs(m_HeadWolfIter->getY() - m_FierceWolfs[i].getY());
				// }else {
				// 	m_FierceWolfs[i].m_Position.m_Y = m_FierceWolfs[i].getY() + StepB[1] * 1;
				// }
				
			
				
			}
		}
	}

	/*
	 * ������: wolfsBesiege
	 * ����: WPA�㷨�е�Χ������
	 * ����:��
	 * ����ֵ: ��
	 */
	void wolfsBesiege()
	{
		//�ⲿ��ԭ��Ϊ������Ⱥ��������ǿʳ���������ڸĳ�һ���̶���������������Ⱥ������
		if (abs(calcFitness(m_HeadWolfIter->m_Position,m_TargetPos)) > 10) {
			cout << "Χ����Ϊ(ʧ��)" << endl;
			drawWofls();
			return;
		}

		int MIN_VALUE = -1;
		int MAX_VALUE = 1;
		for (int i = 0; i < m_ExploringWolfNum; ++i) {//̽��
			int r = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
			double tempX = m_ExploringWolfs[i].getX() + r * StepC[0] * abs(m_TargetPos.m_X - m_ExploringWolfs[i].getX());
			double tempY = m_ExploringWolfs[i].getY() + r * StepC[1] * abs(m_TargetPos.m_Y - m_ExploringWolfs[i].getY());
			if (calcFitness(Point(tempX, tempY), m_TargetPos) > calcFitness(m_ExploringWolfs[i].m_Position, m_TargetPos)) {
				m_ExploringWolfs[i].m_Position = Point(tempX, tempY);
			}
			cout << "Χ����Ϊ��" << "���Ϊ" << m_ExploringWolfs[i].m_index << "��̽�ǣ�" << "x=" << m_ExploringWolfs[i].getX() << ",y=" << m_ExploringWolfs[i].getY() << endl;
		}
		for (int i = 0; i < m_FierceWolfNum; ++i) {//����
			int r = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
			double tempX = m_FierceWolfs[i].getX() + r * StepC[0] * abs(m_TargetPos.m_X - m_FierceWolfs[i].getX());
			double tempY = m_FierceWolfs[i].getY() + r * StepC[1] * abs(m_TargetPos.m_Y - m_FierceWolfs[i].getY());
			if (calcFitness(Point(tempX, tempY), m_TargetPos) > calcFitness(m_FierceWolfs[i].m_Position, m_TargetPos)) {
				m_FierceWolfs[i].m_Position = Point(tempX, tempY);
			}
			cout << "Χ����Ϊ��" << "���Ϊ" << m_FierceWolfs[i].m_index << "�����ǣ�" << "x=" << m_FierceWolfs[i].getX() << ",y=" << m_FierceWolfs[i].getY() << endl;
		}
		drawWofls();
	}


	void run()
	{
		auto start = chrono::steady_clock::now();
		for (int i = 0; i < max_iter; ++i) {
			wolfsWandering();
			if (flag) {
				wolfsCall();
				if (besiege_flag) {
					wolfsBesiege();
					//update();
					if (abs(calcFitness(m_HeadWolfIter->m_Position,m_TargetPos)) < 1.0) {
						break;
					}
					else {
						//�������ߣ�Ҫ�Ա�־����
						flag = false;
						besiege_flag = false;
						//update();
					}
				}
			}
		}
		auto end = chrono::steady_clock::now();
		auto duration = chrono::duration_cast<chrono::seconds>(end - start);
		cout << "��������ʱ��Ϊ��" << duration.count() << "��" << endl;
		cout << "���Ž��Ϊ��" << "x=" << m_HeadWolfIter->getX() << ",y=" << m_HeadWolfIter->getY() << endl;
		cv::waitKey(0);
	}

	void drawWofls()
	{
		cv::Mat img = cv::Mat::zeros(600, 600, CV_8U);
		//����Ϊ��ɫ��ͷ��Ϊ��ɫ��̽��Ϊ��ɫ������Ϊ��ɫ
		int radius = 2;
		circle(img, cv::Point(m_TargetPos.m_X, m_TargetPos.m_Y), radius, cv::Scalar(0, 0, 255), -1);//������
		for (int i = 0; i < m_ExploringWolfs.size(); ++i) {
			circle(img, cv::Point(m_ExploringWolfs[i].getX(), m_ExploringWolfs[i].getY()), radius, cv::Scalar(255, 255, 255), 1);//��̽��
		}
		for(int i=0;i<m_HeadWolfs.size();++i) {
			circle(img, cv::Point(m_HeadWolfs[i].getX(), m_HeadWolfs[i].getY()), radius, cv::Scalar(0, 255, 0), -1);//��ͷ��
		}
		for (int i = 0 ; i < m_FierceWolfs.size(); ++i) {
			circle(img, cv::Point(m_FierceWolfs[i].getX(), m_FierceWolfs[i].getY()), radius, cv::Scalar(255, 0, 0), 1);//������
		}
		cv::imshow("WPA-V1", img);
		cv::waitKey(0.001 * 1000);//�ȴ�0.001��
	}

	


private:
	int m_h = 10;//h������
	double m_alpha = 4;//̽�Ǳ�������
	double m_beta = 6;//��Ⱥ���±�������
	double m_w = 500;//�����ж�����
	int m_WolfNum = 50;//��Ⱥ����
	int m_headWolfNum = 1;
	int m_ExploringWolfNum;//̽����������ȡ[n��(��+1)��n����]֮���������
	int m_FierceWolfNum;//��������
	vector<Wolf> m_HeadWolfs;//ͷ������
	vector<Wolf> m_ExploringWolfs;//̽������
	vector<Wolf> m_FierceWolfs;//��������

	Point m_TargetPos;

	vector<Wolf>::iterator m_HeadWolfIter;//ͷ��ָ��
	vector<Wolf> m_BesiegeWolfs;//Χ������

	vector<int> maxd;//ά�����ֵ
	vector<int> mind;//ά����Сֵ
	vector<double> StepA;//���߲���
	vector<double> StepB;//�ٻ�����
	vector<double> StepC;//Χ������
	double S = 1000;//��������
	int dim = 2;//ά��

	int max_wandering_iter = 30;//������ߴ���
	int max_iter = 1000;//����������

	bool flag = false;//ͷ�Ǹ��±�־	
	bool besiege_flag = false;//Χ����Ϊ��־

};


int main()
{
	WPA wpa(20, 2, 4, 6, 500, 1000, 1000, Point(500, 500));
	wpa.run();
	return 0;
}