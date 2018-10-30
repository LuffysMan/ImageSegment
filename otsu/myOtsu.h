/*
OTSU�㷨С�᣺
	1.��������ͼ��Ĺ�һ��ֱ��ͼ��ʹ��p_i,i=0,1,2,��,L-1��ʾ��ֱ��ͼ�ĸ�������
	2.����k=0,1,2,��,L-1�������ۼƺ�P_1(k)��
	3.����k=0,1,2,��,L-1�������ۼƾ�ֵm(k)��
	4.����ȫ�ֻҶȾ�ֵm_G
	5.����k=0,1,2,��,L-1��������䷽���_B^2 (k)
	6.�õ�Otsu��ֵk*����ʹ����䷽���_B^2 (k)����kֵ��������ֵ��Ψһ������Ӧ��⵽�ĸ������ֵk��ƽ���õ�k*
	7.��[k=k*]������ʽ(3.16)���õ��ɷ��Զ�����*
*/
#pragma once
#include "pch.h"
#define IN
#define OUT
using namespace cv;
//declare
int myOtsu(Mat greyImage, OUT double& k_, OUT double& y_);
int regularize(IN Mat image, OUT double* histgram);
int accumulateSum(IN double* histgram, OUT double*P1);
int accumulateMeans(IN double* histgram, OUT double*m);
int SBetweenClass(IN double* P1, IN double*m, IN double mG, OUT double*SB);
double SGlobal(IN double* histgram, IN double mG);
int bestThreshold(IN double* SB, OUT double& SB_, double& k_);
int mySegment(IN OUT Mat& dst, double threshold);