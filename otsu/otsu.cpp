// otsu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "otsu.h"

using namespace cv;
using namespace std;

//int main()
//{
//	Mat srcimage, grayimage, dstimage;
//	srcimage = imread("liu.jpg");
//	namedWindow("原图", 0);
//	imshow("原图", srcimage);                                            //显示原图
//	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);                         //得到灰度图
//	double time0 = static_cast<double>(getTickCount());                    //记录程序开始时间
//	int thresholdValue = Otsu2D(grayimage);                              //调用二维Otsu函数
//	time0 = ((double)getTickCount() - time0) / getTickFrequency();
//	cout << "算法运行时间为：" << time0 << endl;
//	cout << "Otsu阈值为：" << thresholdValue << endl;
//	threshold(grayimage, dstimage, thresholdValue, 255, THRESH_BINARY);    //将得到的阈值传入函数，得到分割效果图
//	namedWindow("Otsu算法结果", 0);
//	imshow("Otsu算法结果", dstimage);
//	waitKey();
//	return 0;
//}
int Otsu2D(Mat srcimage)
{
	double Histogram[256][256];        //建立二维灰度直方图
	double TrMax = 0.0;                //用于存储矩阵的迹（矩阵对角线之和）
	int height = srcimage.rows;        //矩阵的行数
	int width = srcimage.cols;         //矩阵的列数
	int N = height * width;              //像素的总数
	int T;                             //最终阈值
	uchar *data = srcimage.data;
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			Histogram[i][j] = 0;      //初始化变量
		}
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int Data1 = data[i*srcimage.step + j];         //获取当前灰度值
			int Data2 = 0;                           //用于存放灰度的平均值
			for (int m = i - 1; m <= i + 1; m++)
			{
				for (int n = j - 1; n <= j + 1; n++)
				{
					if ((m >= 0) && (m < height) && (n >= 0) && (n < width))
						Data2 += data[m*srcimage.step + n];//邻域灰度值总和
				}
			}
			Data2 = Data2 / 9;
			Histogram[Data1][Data2]++;                  //记录（i,j）的数量
		}
	}
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			Histogram[i][j] /= N;     //归一化的每一个二元组的概率分布

	double Fgi = 0.0;    //前景区域均值向量i分量
	double Fgj = 0.0;    //前景区域均值向量j分量
	double Bgi = 0.0;    //背景区域均值向量i分量
	double Bgj = 0.0;    //背景区域均值向量j分量
	double Pai = 0.0;    //全局均值向量i分量 panorama(全景)
	double Paj = 0.0;    //全局均值向量j分量
	double w0 = 0.0;     //前景区域联合概率密度
	double w1 = 0.0;     //背景区域联合概率密度
	double num1 = 0.0;   //遍历过程中前景区i分量的值
	double num2 = 0.0;   //遍历过程中前景区j分量的值
	double num3 = 0.0;   //遍历过程中背景区i分量的值
	double num4 = 0.0;   //遍历过程中背景区j分量的值
	int Threshold_s = 0; //阈值s
	int Threshold_t = 0; //阈值t
	double temp = 0.0;   //存储矩阵迹的最大值
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			Pai += i * Histogram[i][j];   //全局均值向量i分量计算
			Paj += j * Histogram[i][j];   //全局均值向量j分量计算
		}
	}
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			w0 += Histogram[i][j];        //前景的概率
			num1 += i * Histogram[i][j];    //遍历过程中前景区i分量的值
			num2 += j * Histogram[i][j];    //遍历过程中前景区j分量的值

			w1 = 1 - w0;                  //背景的概率
			num3 = Pai - num1;            //遍历过程中背景区i分量的值
			num4 = Paj - num2;            //遍历过程中背景区j分量的值

			Fgi = num1 / w0;
			Fgj = num2 / w1;
			Bgi = num3 / w0;
			Bgj = num4 / w1;
			TrMax = ((w0*Pai - num1)*(w0*Pai - num1) + (w0*Paj - num2)*(w0*Paj - num2)) / (w0*w1);
			if (TrMax > temp)
			{
				temp = TrMax;
				Threshold_s = i;
				Threshold_t = j;
			}
		}
	}
	cout << Threshold_s << " " << Threshold_t << endl;
	T = Threshold_s;
	return T;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
