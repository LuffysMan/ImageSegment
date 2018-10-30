#include "pch.h"
#include "myOtsu.h"
#include "otsu.h"
#include <cstring>

using namespace cv;
using namespace std;

int main()
{
	char szModuleFileName[MAX_PATH];								//获取程序所在路径
	char szModuleDir[MAX_PATH];
	GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);
	_splitpath_s(szModuleFileName, NULL, NULL, szModuleDir, MAX_PATH, NULL, NULL, NULL, NULL);
	strcat_s(szModuleDir, MAX_PATH, "liu.jpg");
#if 0
	Mat srcimage, grayimage, dstimage;
	srcimage = imread(szModuleDir);
	namedWindow("原图", 0);
	imshow("原图", srcimage);                                            //显示原图
	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);                         //得到灰度图
	double time0 = static_cast<double>(getTickCount());                    //记录程序开始时间
	int thresholdValue = Otsu2D(grayimage);                              //调用二维Otsu函数
	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << "算法运行时间为：" << time0 << endl;
	cout << "Otsu阈值为：" << thresholdValue << endl;
	threshold(grayimage, dstimage, thresholdValue, 255, THRESH_BINARY);    //将得到的阈值传入函数，得到分割效果图
	namedWindow("Otsu算法结果", 0);
	imshow("Otsu算法结果", dstimage);
	waitKey();

#endif
#if 1

	Mat srcimage, grayimage, dstimage;
	srcimage = imread(szModuleDir);
	namedWindow("原图", 0);
	imshow("原图", srcimage);
	//灰度化
	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);
	//使用Otsu输出阈值
	double thresholdValue = 0;
	double performance = 0;
	myOtsu(grayimage, thresholdValue, performance);
	cout << "最佳阈值：" << thresholdValue << endl;
	cout << "性能度量：" << performance << endl;
	//使用阈值进行分割
	dstimage = grayimage.clone();
	namedWindow("灰度图", 0);
	imshow("灰度图", dstimage);
	mySegment(dstimage, thresholdValue);
	namedWindow("Otsu算法结果", 0);
	imshow("Otsu算法结果", dstimage);
	waitKey();
#endif 

	return 0;
}