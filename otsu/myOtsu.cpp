#include "pch.h"
#include "myOtsu.h"

using namespace cv;
using namespace std;
#define L 256

int myOtsu(Mat greyImage, OUT double& k_, OUT double& y_)
{
	int nRet = 0;
	double histgram[L] = {0};
	//归一化，分为L级
	nRet = regularize(greyImage, histgram);
	//计算累积和P1(k)
	double P1[L] = { 0 };
	nRet = accumulateSum(histgram, P1);
	//3.对于k = 0, 1, 2, …, L - 1，计算累计均值m(k)。
	double m[L] = { 0 };
	nRet = accumulateMeans(histgram, m);
	//4.计算全局灰度均值m_G
	double mG = m[L-1];
	//5.对于k = 0, 1, 2, …, L - 1，计算类间方差σ_B ^ 2 (k)和全局方差σ_G ^ 2
	double SB[L] = { 0 };
	nRet = SBetweenClass(P1, m, mG, SB);
	double SG = 0;
	SG = SGlobal(histgram, mG);
	//6.得到Otsu阈值k*，即使得类间方差σ_B ^ 2 (k)最大的k值。如果最大值不唯一，用相应检测到的各个最大值k的平均得到k*
	double SB_ = 0;
	nRet = bestThreshold(SB, SB_, k_);
	//7.在[k = k * ]处计算式(3.16)，得到可分性度量γ*
	y_ = SB_ / SG;

	return nRet;
}

//归一化，分为L级
int regularize(IN Mat image, OUT double* histgram)
{
	int height = image.rows;
	int width = image.cols;
	int MN = height * width;
	uchar* data = image.data;
	int pV = 0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			pV = data[i*image.step + j];
			histgram[pV] += 1;							//该灰度值对应位置计数+1
		}
	}
	//归一化
	for (int i = 0; i < L; ++i)
	{
		histgram[i] /= MN;
	}
	return 1;
}
//计算累积和P_1(k)
int accumulateSum(IN double* histgram, OUT double*P1)
{
	P1[0] = histgram[0];
 	for (int i = 1; i < L; ++i)
	{
		P1[i] = P1[i-1] + histgram[i];
	}
	return 1;
}
//3.对于k = 0, 1, 2, …, L - 1，计算累计均值m(k)。
int accumulateMeans(IN double* histgram, OUT double*m)
{
	m[0] = 0 * histgram[0];
	for (int i = 1; i < L; ++i)
	{
		m[i] = m[i - 1] + i * histgram[i];
	}
	return 1;
}
//5.对于k = 0, 1, 2, …, L - 1，计算类间方差σ_B ^ 2 (k)
int SBetweenClass(IN double* P1, IN double*m, IN double mG, OUT double*SB)
{
	double P2 = 0;
	for (int i = 0; i < L; ++i)
	{
		P2 = 1 - P1[i];
		SB[i] = mG*P1[i] - m[i];
		SB[i] = pow(SB[i], 2);
		SB[i] = SB[i] / (P1[i] * P2);
	}
	return 1;
}
//5.对于k = 0, 1, 2, …, L - 1，计算全局方差σ_G ^ 2
double SGlobal(IN double* histgram, IN double mG)
{
	double sG = 0;
	double sGI = 0;
	for (int i = 0; i < L; ++i)
	{
		sGI = i - mG;
		sGI = pow(sGI, 2);
		sGI *= histgram[i];
		sG += sGI;
	}
	return sG;
}
//6.得到Otsu阈值k*，即使得类间方差σ_B ^ 2 (k)最大的k值。如果最大值不唯一，用相应检测到的各个最大值k的平均得到k*
int bestThreshold(IN double* SB, OUT double& SB_, double& k_)
{
	map<int,double> mapMax;
	mapMax.insert(make_pair(0, SB[0]));
	map<int, double>::iterator iter;
	unsigned long long N = 0;
	for (int i = 1; i < L; ++i)
	{
		iter = mapMax.begin();
		if (iter->second < SB[i])
		{
			mapMax.clear();
			mapMax.insert(make_pair(i,SB[i]));
		}
		else if (iter->second == SB[i])
		{
			mapMax.insert(make_pair(i, SB[i]));
		}
	}
	N = mapMax.size();
	SB_ = iter->second;
	for (iter = mapMax.begin(); iter != mapMax.end(); ++iter)
	{
		k_ += iter->first;
	}
	k_ /= N;
	return 1;
}
//使用阈值进行分割
int mySegment(IN OUT Mat& dst, double threshold)
{
	int height = dst.rows;
	int width = dst.cols;
	size_t nP = 0;
	uchar* data = dst.data;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			nP = i * dst.step + j;
			data[nP] = data[nP] > threshold ? 255 : 0;
		}
	}
	return 1;
}


//int main()
//{
//	Mat srcImage, grayImage, dstImage;
//	String imgPath("liu.jpg");
//	//read image
//	srcImage = imread(imgPath);
//	//灰度化
//	cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//	//使用Otsu输出阈值
//	double threshold = 0;
//	double performance = 0;
//	threshold = myOtsu(grayImage, threshold, performance);
//	//使用阈值进行分割
//	segment(grayImage, dstImage, threshold);
//	imshow("segment result", dstImage);
//
//}