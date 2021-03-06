// ImageSegment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "CommonFunc.h"
#include "CPerformance.h"
#include "CPersistent.h"
#include <fstream>

using namespace std;
using namespace cv;
using namespace persistent;

void winnerTakeAll();
void makeGroundTruth();
void calPerformance();
void test_serializer();

int main()
{
	while (true)
	{
		char key(0);
		cin >> key;
		if ('q' == key) break;
		else if( key == '0')
		{
			CGetSpecificTime tMeasure;
			tMeasure.start();
			winnerTakeAll();				//执行winner-take-all算法
			tMeasure.end();
			cout << "winner执行时间" << tMeasure.GetSpecificTime() << endl;
		}
		else if (key == '1')
		{
			//makeGroundTruth();
		}
		else if (key == '2')
		{
			calPerformance();
		}
		else if (key == '3')
		{
			test_serializer();
		}

	}
	waitKey();
	return 0;
}

void winnerTakeAll()
{
	//获取图像路径并读图
	char imgDir[_MAX_PATH] = { 0 };
	char imgPath[_MAX_PATH] = { 0 };
	vector<char*> vecSzFilename;
	getWorkDir(imgDir);
	sprintf_s(imgDir, _MAX_PATH, "%s%s", imgDir, "image\\");
	listFiles(imgDir, vecSzFilename);
	//分别用k-means聚类
	Mat imgSrc = imread(vecSzFilename.at(0));
	//imshow("srcImage", imgSrc);
#if 1
	CGetSpecificTime tMeasure;
	tMeasure.start();
	//k-means
	Mat imgKmeans = imgSrc.clone();
	myKmeans(imgKmeans, 4);				//K = 4
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageK.dat");		//写入dat文件保真, jpg文件读取出来之后像素值有细微变化
	serialize(imgKmeans, imgPath);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageK.jpg");
	imwrite(imgPath, imgKmeans);
	tMeasure.end();
	cout << "kmeans执行时间" << tMeasure.GetSpecificTime() << endl;
#endif
#if 1
	tMeasure.start();
	//均值漂移聚类
	Mat imgMeanShift = imgSrc.clone();
	myMeanShiftFiltering(imgSrc, imgMeanShift);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageM.dat");
	serialize(imgMeanShift, imgPath);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageM.jpg");
	imwrite(imgPath, imgMeanShift);
	tMeasure.end();
	cout << "meanshift执行时间" << tMeasure.GetSpecificTime() << endl;
#endif
#if 1
	vector<COLORBLOCK> vecBlockM;
	Mat blockM(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	countBlocks(imgMeanShift, vecBlockM, blockM);					
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageBM.dat");		//将分块后的图保存
	serialize(blockM, imgPath);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageBM.jpg");
	imwrite(imgPath, blockM);
	//4每一个块的候选人色彩
	Vec3b colorTab[] =
	{
		Vec3b(255, 0, 0),			//蓝
		Vec3b(0, 255, 0),			//绿
		Vec3b(0, 0, 255),			//红
		Vec3b(0, 255, 255),			//黄
	};
	ColorCandidate candidates[4];
	candidates[0].color = colorTab[0];
	candidates[1].color = colorTab[1];
	candidates[2].color = colorTab[2];
	candidates[3].color = colorTab[3];
	for (int i = 0; i < vecBlockM.size(); ++i)
	{
		//drawBlock(vecBlockM.at(i), i);
		//统计候选色彩票数
		countVotes(imgKmeans, vecBlockM.at(i), candidates);
		//findCandidate(imgKmeans, BlockM, candidates);
		//赢者通吃
		eatAll(imgMeanShift, vecBlockM.at(i), candidates);
		//eatAll(blockM, vecBlockM.at(i), candidates);
	}
	//imshow("eatall", blockM);
	//sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageW.dat");		//将分块后的图保存
	//serialize(imgMeanShift, imgPath);
	//sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageW.jpg");
	//imwrite(imgPath, imgMeanShift);
#endif
}

void makeGroundTruth()
{
	//获取图像路径并读图
	char imgDir[_MAX_PATH] = { 0 };
	char imgPath[_MAX_PATH] = { 0 };
	getWorkDir(imgDir);
	sprintf_s(imgDir, _MAX_PATH, "%s%s", imgDir, "image\\");
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageGT.jpg");
	Mat imgA = imread(imgPath);
	//forGroundTruth(imgA, Vec3b(0, 255, 0), Vec3b(0, 255, 0));
	//forGroundTruth(imgA, Vec3b(0, 0, 255), Vec3b(0, 0, 255));
	//forGroundTruth(imgA, Vec3b(255, 0, 0), Vec3b(255, 0, 0));
	forGroundTruthEx(imgA);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageGT.dat");
	serialize(imgA, imgPath);
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageGT.jpg");
	imwrite(imgPath, imgA);

	//sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "b.jpg");
	//Mat imgB = imread(imgPath);
	//forGroundTruth(imgA, imgB, Vec3b(0, 0, 0), Vec3b(0, 255, 255));
	//imshow("imageB", imgB);
	//sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "bx.jpg");
	//imwrite(imgPath, imgB);
}
/*
@计算图像分割性能指标 IOU, precesion, recall
*/
void calPerformance()
{
	//获取图像路径并读图
	char imgDir[_MAX_PATH] = { 0 };
	char imgPath[_MAX_PATH] = { 0 };
	getWorkDir(imgDir);
	sprintf_s(imgDir, _MAX_PATH, "%s%s", imgDir, "image\\");
	//从dat文件恢复处理ground truth 和 winner-take-all 分割后的图像
	Mat mGT(641, 1024, CV_8UC3, Scalar(0,0,0));
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageGT.dat");
	deserialize(mGT, imgPath);

	CPerformance cal;
	ColorBlock GT;
	ColorBlock SEG;
	vector<COLORBLOCK> vecBlockG;
	Mat blockG(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	countBlocksEx(mGT, vecBlockG, blockG);
	imshow("块重建", blockG);
	//计算winner的三个性能系数
	Mat mKm(641, 1024, CV_8UC3, Scalar(0, 0, 0));
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageBM.dat");
	deserialize(mKm, imgPath);
	vector<COLORBLOCK> vecBlockK;
	Mat blockK(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	countBlocksEx(mKm, vecBlockK, blockK);
	GT = vecBlockG.at(0);
	SEG = vecBlockK.at(1);
	cal.setGT_SEG(GT, SEG);
	float fIou = cal.calIou();
	float fPre = cal.calPrecesion();
	float fRec = cal.calRecall();
	cout << "fiou" << fIou << "fpre" << fPre << "frec" << fRec << endl;

	GT = vecBlockG.at(1);
	SEG = vecBlockK.at(4);
	cal.setGT_SEG(GT, SEG);
	fIou = cal.calIou();
	fPre = cal.calPrecesion();
	fRec = cal.calRecall();
	cout << "fiou" << fIou << "fpre" << fPre << "frec" << fRec << endl;

	GT = vecBlockG.at(2);
	SEG = vecBlockK.at(12);
	cal.setGT_SEG(GT, SEG);
	fIou = cal.calIou();
	fPre = cal.calPrecesion();
	fRec = cal.calRecall();
	cout << "fiou" << fIou << "fpre" << fPre << "frec" << fRec << endl;

	GT = vecBlockG.at(3);
	SEG = vecBlockK.at(2);
	cal.setGT_SEG(GT, SEG);
	fIou = cal.calIou();
	fPre = cal.calPrecesion();
	fRec = cal.calRecall();
	cout << "fiou" << fIou << "fpre" << fPre << "frec" << fRec << endl;
	//计算kmeans的三个系数

	//计算meanshift的三个系数
	waitKey();
}


void test_serializer()
{
	//获取图像路径并读图
	char imgDir[_MAX_PATH] = { 0 };
	char imgPath[_MAX_PATH] = { 0 };
	getWorkDir(imgDir);
	sprintf_s(imgDir, _MAX_PATH, "%s%s", imgDir, "image\\");
	sprintf_s(imgPath, _MAX_PATH, "%s%s", imgDir, "imageGT.dat");
	Mat imOut(641, 1024, CV_8UC3, Scalar(255, 0, 0));
	Mat imIn(641, 1024, CV_8UC3, Scalar(0, 255, 0));
	serialize(imOut, imgPath);
	imshow("写出图像", imOut);
	deserialize(imIn, imgPath);
	imshow("读入图像", imIn);
	waitKey();
}