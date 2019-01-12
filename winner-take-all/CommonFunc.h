#pragma once
#include <direct.h>
#include <io.h>
#include "DataType.h"

//枚举指定路径下文件，c++方法
void listFiles(char* dir, vector<char*>& vecSzFilename);
//枚举指定路径下文件夹，c++方法
void listDir(char* dir, vector<char*>& vecSzDir);
//获取程序所在文件夹，c++方法
int getWorkDir(char* szProgramPath);
//获取精确时间（微秒级），可用于测试性能
double getSpecificTime();

//opencv Mat和普通uchar数组互转
cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB = 1);
int TransMatToBuffer(cv::Mat mSrc, unsigned char*& ppBuffer, int& nWidth, int& nHeight, int& nBandNum, int& nBPB, size_t& nMemSize);
//高斯滤波
void myGaussian(const Mat _src, Mat& _dst);
double **getGuassionArray(int size, double sigma);
//kmeans聚类
int myKmeans(Mat pic, int K);
//均值漂移聚类
int myMeanShiftFiltering(Mat img, Mat& res);
//绘制灰度直方图
int visulizeHist(Mat srcImage);
//构建颜色块
int countBlocks(Mat mSrc, COLORBLOCK& cb);
int countBlocks(Mat mSrc, vector<COLORBLOCK>& vcb, Mat& blockM);
int countBlocksEx(Mat mSrc, vector<COLORBLOCK>& vcb, Mat& blockM);
int drawBlock(ColorBlock cb, int index);
//统计候选色彩票数
int countVotes(Mat srcImage, COLORBLOCK& cb, ColorCandidate* cc);
//赢者通吃
int eatAll(Mat& Msrc, COLORBLOCK& cb, ColorCandidate* cc);
//两种颜色几乎相同，如果BGR三个通道数值相差均不超过range，则认为相同
int almostSame(Vec3b ca, Vec3b cb, int range = 10);	
//两种颜色几乎相同，如果BGR三个通道数值相差均不超过range，则认为相同
int almostSameEx(Vec3b ca, Vec3b cb, float range = 0.9);
//将非背景像素统统标注为一种颜色,用于制作ground truth
int forGroundTruth(Mat src, Vec3b bgc, Vec3b fc);
int forGroundTruthEx(Mat src);
//将非背景像素统统标注为一种颜色,用于制作ground truth
int forGroundTruth(Mat src, Mat dst, Vec3b bgc, Vec3b fc);
//序列化一幅图像到dat文件
int serialize(Mat src, char* filename);
int deserialize(Mat& dst, char* filename);

class CGetSpecificTime
{
public:
	 CGetSpecificTime();
	 ~CGetSpecificTime();
	 void start();
	 void end();
	 double GetSpecificTime();
	 double GetTime();
private:
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;

};












