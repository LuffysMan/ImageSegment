#include "pch.h"
#include "COtsu.h"
using namespace cv;
using namespace std;

#define L 256

COtsu::COtsu()
{
}

COtsu::COtsu(Mat& srcImage)
{
	this->srcimage = srcImage;
}

COtsu::COtsu(char * imgPath)
{
	this->srcimage = imread(imgPath);
}


COtsu::~COtsu()
{
}

int COtsu::singleThreshold(Mat greyImage, OUT double & k_, OUT double & y_)
{
	int nRet = 0;
	double histgram[L] = { 0 };
	//归一化，分为L级
	nRet = histogram(greyImage, histgram);
	//计算累积和P1(k)
	double P1[L] = { 0 };
	nRet = accumulateSum(histgram, P1);
	//3.对于k = 0, 1, 2, …, L - 1，计算累计均值m(k)。
	double m[L] = { 0 };
	nRet = accumulateMeans(histgram, m);
	//4.计算全局灰度均值m_G
	double mG = m[L - 1];
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

int COtsu::multiThreshold(Mat grayImage, OUT vector<double>& vecThreshold)
{
	//计算灰度级直方图
	double hist[L] = { 0 };
	histogram(grayimage, hist);
	//计算类间方差矩阵
	double P1 = 0;
	double P2 = 0;
	double P3 = 0;
	double m1 = 0;
	double m2 = 0;
	double m3 = 0;
	double mG = 0;
	int k1 = 0;			//阈值
	int k2 = 0;
	typedef struct var_thrshd 
	{
		var_thrshd() :sigmaB(0), k1(0), k2(0) {};
		var_thrshd(double SB, int K1, int K2) :sigmaB(SB), k1(K1), k2(K2) {};
		double sigmaB = 0;
		int k1 = 1;
		int k2 = 2;
	}VAR_THRSHD,*pVAR_THRSHD;
	vector<VAR_THRSHD> vecVariance;
	vector<VAR_THRSHD>::iterator iterVec;
	//for (int i = 0; i < L - 1; ++i)
	//{
	//	mG += i * hist[i];
	//}
	//计算类间方差
	for (int k1 = 1; k1 <= 253; ++k1)
	{
		//计算P1,m1
		for (int i = 0; i <= k1; ++i)
		{
			P1 += hist[i];
			m1 += i * hist[i];
		}
		m1 /= P1;
		if (isnan(m1)) { m1 = 0.0; };			//出现了0/0的情况，结果应该为0
		for (int k2 = k1 + 1; k2 <= 254; ++k2)
		{
			//计算P2,m2
			for (int i = k1 + 1; i <= k2; ++i)
			{
				P2 += hist[i];
				m2 += i * hist[i];
			}
			m2 /= P2;
			if (isnan(m2)) { m2 = 0.0; };			//出现了0/0的情况，结果应该为0
			//计算P3,m3
			for (int i = k2 + 1; i <= 255; ++i)
			{
				P3 += hist[i];
				m3 += i * hist[i];
			}
			m3 /= P3;
			if (isnan(m3)) { m3 = 0.0; };			//出现了0/0的情况，结果应该为0
			//计算mG
			mG = P1 * m1 + P1 * m2 + P3 * m3;
			//计算类间方差
			double variance = 0;
			variance = P1 * (mG-m1)*(mG - m1) + P2 * (mG - m2)*(mG - m2) + P3 * (mG - m3)*(mG - m3);
			VAR_THRSHD sVarThrshd(variance, k1, k2);
			vecVariance.push_back(sVarThrshd);			//将类间方差和对应阈值压入map
		}
	}
	//选择最大的方差,求平均阈值
	double maxVar = 0;
	double maxVarAvrg = 0;
	int cnt = 0;
	iterVec = vecVariance.begin();
	maxVar = iterVec->sigmaB;
	int _k1 = 0;
	int _k2 = 0;
	for (iterVec = vecVariance.begin(); iterVec != vecVariance.end(); ++iterVec)
	{
		//maxVar = maxVar >= iterVec->sigmaB ? maxVar : iterVec->sigmaB;
		if (maxVar < iterVec->sigmaB)
		{
			maxVar = iterVec->sigmaB;
			_k1 = iterVec->k1;
			_k2 = iterVec->k2;
		}
	}
	for (iterVec = vecVariance.begin(); iterVec != vecVariance.end(); ++iterVec)
	{
		if (maxVar == iterVec->sigmaB)
		{
			k1 += iterVec->k1;
			k2 += iterVec->k2;
			++cnt;
		}
	}
	k1 /= cnt;
	k2 /= cnt;
	vecThreshold.push_back(k1);
	vecThreshold.push_back(k2);
	return 0;
}

int COtsu::histogram(IN Mat image, OUT double * histgram)
{
	int height = image.rows;
	int width = image.cols;
	int MN = height * width;
	uchar* data = image.data;
	//int pV = 0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//pV = data[i*image.step + j];
			histgram[*data]++;							//该灰度值对应位置计数+1
			++data;
		}
	}
	//找出低谷值
	for (int i = 0; i < L - 1; ++i)
	{
		double back = 0;
		double fore = 0;
		for (int j = 1; j <= 10; ++j)
		{
			if ((i - j) >= 0)
			{
				back = histgram[i] - histgram[i - j];
			}
			if ((i + j) <= L-1)
			{
				fore = histgram[i] - histgram[i + j];
			}
		}
		if (back <= 0 && fore <= 0)
		{
			cout << "低谷" << i << endl;
		}
	}
	//归一化
	for (int i = 0; i < L; ++i)
	{
		histgram[i] /= MN;
	}
	return 0;
}

int COtsu::accumulateSum(IN double * histgram, OUT double * P1)
{
	P1[0] = histgram[0];
	for (int i = 1; i < L; ++i)
	{
		P1[i] = P1[i - 1] + histgram[i];
	}
	return 0;
}

int COtsu::accumulateMeans(IN double * histgram, OUT double * m)
{
	m[0] = 0 * histgram[0];
	for (int i = 1; i < L; ++i)
	{
		m[i] = m[i - 1] + i * histgram[i];
	}
	return 0;
}

int COtsu::SBetweenClass(IN double * P1, IN double * m, IN double mG, OUT double * SB)
{
	double P2 = 0;
	for (int i = 0; i < L; ++i)
	{
		P2 = 1 - P1[i];
		SB[i] = mG * P1[i] - m[i];
		SB[i] = pow(SB[i], 2);
		SB[i] = SB[i] / (P1[i] * P2);
		if (isnan(SB[i]))					//出现了0/0的情况，原公式为书上第481页类间方差公式（10.3-14），可以证明此种情况值为0
		{
			SB[i] = 0;
		}
	}
	return 0;
}

double COtsu::SGlobal(IN double * histgram, IN double mG)
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

int COtsu::bestThreshold(IN double * SB, OUT double & SB_, double & k_)
{
	map<int, double> mapMax;
	mapMax.insert(make_pair(0, SB[0]));
	map<int, double>::iterator iter;
	unsigned long long N = 0;
	for (int i = 1; i < L; ++i)
	{
		iter = mapMax.begin();
		if (iter->second < SB[i])
		{
			mapMax.clear();
			mapMax.insert(make_pair(i, SB[i]));
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
	return 0;
}

int COtsu::mySegment(IN OUT Mat & dst, double threshold)
{
	int height = dst.rows;
	int width = dst.cols;
	size_t nP = 0;
	uchar* data = dst.data;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//nP = i * dst.step + j;
			//data[nP] = data[nP] > threshold ? 255 : 0;
			*data = *data > threshold ? 255 : 0;
		}
	}
	return 0;
}

int COtsu::mySegment(IN OUT Mat & dst, double _k1, double _k2)
{
	int gray1 = 0;
	int gray2 = 127;
	int gray3 = 255;
	int height = dst.rows;
	int width = dst.cols;
	size_t nP = 0;
	uchar* data = dst.data;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//nP = i * dst.step + j;
			//data[nP] = data[nP] > threshold ? 255 : 0;
			if (0 <= *data&&*data < _k1)
			{
				*data = (byte)gray1;
			}
			else if (_k1 <= *data&&*data < _k2)
			{
				*data = (byte)gray2;
			}
			else
			{
				*data = (byte)gray3;
			}
			data++;
		}
	}
	return 0;
}

int COtsu::run()
{
	//Mat srcimage, grayimage, dstimage;
	//srcimage = imread(szModuleDir);
	namedWindow("原图", 0);
	imshow("原图", srcimage);
	int nTemp = srcimage.channels();
	//灰度化
	cvtColor(srcimage, grayimage, COLOR_RGB2GRAY);
	namedWindow("灰度图", 0);
	imshow("灰度图", grayimage);
	visulizeHist(grayimage);
#if 1
	vector<double> vecThreshold;
	multiThreshold(grayimage, vecThreshold);
	dstimage = grayimage.clone();
	mySegment(dstimage, vecThreshold.at(0), vecThreshold.at(1));
	//mySegment(dstimage,156, 195);
	cout << vecThreshold.at(0) << "  " << vecThreshold.at(1) << endl;
	namedWindow("分割", 0);
	imshow("分割", dstimage);
#endif // 1

#if 0
	//使用高斯滤波对噪声图像进行处理
	Mat imgDenoised;
	GaussianBlur(srcimage, imgDenoised, Size(3, 3), 101, 101);
	namedWindow("平滑", 0);
	imshow("平滑", imgDenoised);
	//kmeans聚类
	myKmeans(imgDenoised, 5);
	namedWindow("聚类", 0);
	imshow("聚类", imgDenoised);
	//使用Otsu输出阈值
	double thresholdValue = 0;
	double performance = 0;
	singleThreshold(imgDenoised, thresholdValue, performance);
	cout << "最佳阈值：" << thresholdValue << endl;
	cout << "性能度量：" << performance << endl;
	//使用阈值进行分割
	dstimage = imgDenoised.clone();
	thresholdValue = 100;
	mySegment(dstimage, thresholdValue);
	namedWindow("Otsu算法结果", 0);
	imshow("Otsu算法结果", dstimage);
#endif // 0

	waitKey();
	return 0;
}

Mat COtsu::GetSrcImage()
{
	return this->srcimage;
}

int COtsu::SetSrcImage(Mat& srcImage)
{
	this->srcimage = srcImage;
	return 0;
}

Mat COtsu::GetGrayImage()
{
	return this->grayimage;
}

Mat COtsu::GetDstImage()
{
	return this->dstimage;
}

