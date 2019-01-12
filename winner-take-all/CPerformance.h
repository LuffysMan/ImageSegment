#pragma once
#include "pch.h"

class CPerformance
{
public:
	CPerformance();
	~CPerformance();
public:
	void setGT_SEG(ColorBlock& GT, ColorBlock& SEG);
	float calIou();
	float calPrecesion();
	float calRecall();
private:
	int IandU(ColorBlock& GT, ColorBlock& SEG, ColorBlock& BI, ColorBlock& BU);

private:
	ColorBlock GT;
	ColorBlock SEG;
	float iou;
	float precesion;
	float recall;
};

