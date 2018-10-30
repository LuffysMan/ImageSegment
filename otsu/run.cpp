#include "pch.h"
#include "myOtsu.h"
#include "otsu.h"
#include <cstring>

using namespace cv;
using namespace std;

int main()
{
	char szModuleFileName[MAX_PATH];								//��ȡ��������·��
	char szModuleDir[MAX_PATH];
	GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);
	_splitpath_s(szModuleFileName, NULL, NULL, szModuleDir, MAX_PATH, NULL, NULL, NULL, NULL);
	strcat_s(szModuleDir, MAX_PATH, "liu.jpg");
#if 0
	Mat srcimage, grayimage, dstimage;
	srcimage = imread(szModuleDir);
	namedWindow("ԭͼ", 0);
	imshow("ԭͼ", srcimage);                                            //��ʾԭͼ
	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);                         //�õ��Ҷ�ͼ
	double time0 = static_cast<double>(getTickCount());                    //��¼����ʼʱ��
	int thresholdValue = Otsu2D(grayimage);                              //���ö�άOtsu����
	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << "�㷨����ʱ��Ϊ��" << time0 << endl;
	cout << "Otsu��ֵΪ��" << thresholdValue << endl;
	threshold(grayimage, dstimage, thresholdValue, 255, THRESH_BINARY);    //���õ�����ֵ���뺯�����õ��ָ�Ч��ͼ
	namedWindow("Otsu�㷨���", 0);
	imshow("Otsu�㷨���", dstimage);
	waitKey();

#endif
#if 1

	Mat srcimage, grayimage, dstimage;
	srcimage = imread(szModuleDir);
	namedWindow("ԭͼ", 0);
	imshow("ԭͼ", srcimage);
	//�ҶȻ�
	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);
	//ʹ��Otsu�����ֵ
	double thresholdValue = 0;
	double performance = 0;
	myOtsu(grayimage, thresholdValue, performance);
	cout << "�����ֵ��" << thresholdValue << endl;
	cout << "���ܶ�����" << performance << endl;
	//ʹ����ֵ���зָ�
	dstimage = grayimage.clone();
	namedWindow("�Ҷ�ͼ", 0);
	imshow("�Ҷ�ͼ", dstimage);
	mySegment(dstimage, thresholdValue);
	namedWindow("Otsu�㷨���", 0);
	imshow("Otsu�㷨���", dstimage);
	waitKey();
#endif 

	return 0;
}