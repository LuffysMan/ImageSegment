#include "pch.h"
#include "CPerformance.h"


CPerformance::CPerformance():iou(0), precesion(0), recall(0)
{
}


CPerformance::~CPerformance()
{
}
/*
@要求GT和SEG除了关注的部分,其他为0
*/
void CPerformance::setGT_SEG(ColorBlock & GT, ColorBlock & SEG)
{
	this->GT = GT;
	this->SEG = SEG;
}


float CPerformance::calIou()
{
	ColorBlock Sec;
	ColorBlock Union;
	IandU(GT, SEG, Sec, Union);
	Sec.getCount();
	Union.getCount();
	return float(Sec.nCount)/ Union.nCount;
}

float CPerformance::calPrecesion()
{
	ColorBlock Sec;
	ColorBlock Union;
	IandU(GT, SEG, Sec, Union);
	Sec.getCount();
	return float(Sec.nCount) / SEG.nCount;
}

float CPerformance::calRecall()
{
	ColorBlock Sec;
	ColorBlock Union;
	IandU(GT, SEG, Sec, Union);
	Sec.getCount();
	return float(Sec.nCount) / GT.nCount;
}

int CPerformance::IandU(ColorBlock & GT, ColorBlock & SEG, ColorBlock& BI, ColorBlock& BU)
{
	for (int i = 0; i < GT.nLen; ++i)
	{
		BI.block[i] = GT.block[i] & SEG.block[i];
		BU.block[i] = GT.block[i] | SEG.block[i];
	}
	return 0;
}
