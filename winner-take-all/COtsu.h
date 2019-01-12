/*
OTSU算法小结：
	1.计算输入图像的归一化直方图。使用p_i,i=0,1,2,…,L-1表示该直方图的各个分量
	2.对于k=0,1,2,…,L-1，计算累计和P_1(k)。
	3.对于k=0,1,2,…,L-1，计算累计均值m(k)。
	4.计算全局灰度均值m_G
	5.对于k=0,1,2,…,L-1，计算类间方差σ_B^2 (k)
	6.得到Otsu阈值k*，即使得类间方差σ_B^2 (k)最大的k值。如果最大值不唯一，用相应检测到的各个最大值k的平均得到k*
	7.在[k=k*]处计算式(3.16)，得到可分性度量γ*
*/
#pragma once
#include "pch.h"
#define IN
#define OUT
using namespace cv;

class COtsu
{
public:
	COtsu();
	COtsu(Mat& srcImage);
	COtsu(char* imgPath);
	~COtsu();
//邻域函数
private:				
	int singleThreshold(Mat greyImage, OUT double& k_, OUT double& y_);
	int multiThreshold(Mat grayImage, OUT vector<double>& vecThreshold);
	int histogram(IN Mat image, OUT double* histgram);
	int accumulateSum(IN double* histgram, OUT double*P1);
	int accumulateMeans(IN double* histgram, OUT double*m);
	int SBetweenClass(IN double* P1, IN double*m, IN double mG, OUT double*SB);
	double SGlobal(IN double* histgram, IN double mG);
	int bestThreshold(IN double* SB, OUT double& SB_, double& k_);
	int mySegment(IN OUT Mat& dst, double threshold);
	int mySegment(IN OUT Mat& dst, double _k1, double _k2);
public:
	int run();
	//类通用函数
public:
	Mat GetSrcImage();
	int SetSrcImage(Mat& srcImage);
	Mat GetGrayImage();
	Mat GetDstImage();
private:
	Mat srcimage;
	Mat grayimage;
	Mat dstimage;


};

