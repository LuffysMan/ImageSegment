#include "pch.h"
#include "CommonFunc.h"

void listFiles(char * dir, vector<char*>& vecSzFilename)
{
	char szDir[_MAX_PATH] = { 0 };
	sprintf_s(szDir, _MAX_PATH, "%s%s", dir, "*.*");			//±éÀúÇ°¼ÓÉÏÍ¨Åä·û
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(szDir, &findData);    // ²éÕÒÄ¿Â¼ÖÐµÄµÚÒ»¸öÎÄ¼þ
	if (handle == -1)
	{
		cout << "Failed to find first file!\n";
		return;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			|| strcmp(findData.name, ".") == 0
			|| strcmp(findData.name, "..") == 0
			)    // ÊÇ·ñÊÇ×ÓÄ¿Â¼²¢ÇÒ²»Îª"."»ò".."
		{
			//cout << findData.name << "\t<dir>\n";
		}
		else
		{
			//cout << findData.name << "\t" << findData.size << endl;
			char* pBuffer = new char[_MAX_PATH]();
			sprintf_s(pBuffer, _MAX_PATH, "%s%s", dir, findData.name);
			vecSzFilename.push_back(pBuffer);
		}
	} while (_findnext(handle, &findData) == 0);    // ²éÕÒÄ¿Â¼ÖÐµÄÏÂÒ»¸öÎÄ¼þ

	//cout << "Done!\n";
	_findclose(handle);    // ¹Ø±ÕËÑË÷¾ä±ú
}
//Ã¶¾ÙÖ¸¶¨Â·¾¶ÏÂÎÄ¼þ¼Ð
void listDir(char * dir, vector<char*>& vecSzDir)
{
	char szDir[_MAX_PATH] = { 0 };
	sprintf_s(szDir, _MAX_PATH, "%s%s", dir, "*.*");			//±éÀúÇ°¼ÓÉÏÍ¨Åä·û
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(szDir, &findData);    // ²éÕÒÄ¿Â¼ÖÐµÄµÚÒ»¸öÎÄ¼þ
	if (handle == -1)
	{
		cout << "Failed to find first file!\n";
		return;
	}
	do
	{
		if (findData.attrib & _A_SUBDIR && 
			strcmp(findData.name, ".") != 0 &&
			strcmp(findData.name, "..") != 0)// Ö»¹Ø×¢×ÓÄ¿Â¼
		{
			cout << findData.name << "\t<dir>\n";
			char* pBuffer = new char[_MAX_PATH]();
			sprintf_s(pBuffer, _MAX_PATH, "%s%s%s", dir, findData.name, "\\");
			vecSzDir.push_back(pBuffer);
		}
		else
		{
			////cout << findData.name << "\t" << findData.size << endl;
			//char* pBuffer = new char[_MAX_PATH]();
			//sprintf_s(pBuffer, _MAX_PATH, "%s%s", dir, findData.name);
			//vecSzFilename.push_back(pBuffer);
		}
	} while (_findnext(handle, &findData) == 0);    // ²éÕÒÄ¿Â¼ÖÐµÄÏÂÒ»¸öÎÄ¼þ
	_findclose(handle);    // ¹Ø±ÕËÑË÷¾ä±ú
}

int getWorkDir(char* szProgramPath)
{
	char szPath[_MAX_PATH] = { 0 };
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	char szFname[_MAX_FNAME] = { 0 };
	char szExt[_MAX_EXT] = { 0 };

	GetModuleFileNameA(NULL, szPath, sizeof(szPath));
	//ZeroMemory(szProgramPath, strlen(szProgramPath));
	_splitpath_s(szPath, szDrive, szDir, szFname, szExt);
	sprintf_s(szProgramPath, _MAX_PATH, "%s%s", szDrive, szDir);

	return 0;
}

double getSpecificTime()
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	Sleep(1000);							//此处插入要检测的程序代码
	QueryPerformanceCounter(&nEndTime);
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	return time;
}

cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB)
{
	cv::Mat mDst;
	if (nBandNum == 4)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC4);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC4);
		}
	}
	else if (nBandNum == 3)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC3);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC3);
		}
	}
	else if (nBandNum == 1)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC1);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC1);
		}
	}

	for (int j = 0; j < nHeight; ++j)
	{
		unsigned char* data = mDst.ptr<unsigned char>(j);
		//unsigned char* pSubBuffer = pBuffer + (nHeight - 1 - j) * nWidth * nBandNum * nBPB;		//{modified by yuecui 2018/11/21
		unsigned char* pSubBuffer = pBuffer + j * nWidth * nBandNum * nBPB;			//}end modify yuecui 2018/11/21
		memcpy(data, pSubBuffer, nWidth * nBandNum * nBPB);
	}
	if (nBandNum == 1)
	{
		cv::cvtColor(mDst, mDst, CV_GRAY2BGR);
	}
	else if (nBandNum == 3)
	{
		cv::cvtColor(mDst, mDst, CV_RGB2BGR);
	}
	else if (nBandNum == 4)
	{
		cv::cvtColor(mDst, mDst, CV_RGBA2BGR);
	}

	return mDst;
}

int TransMatToBuffer(cv::Mat mSrc, unsigned char*& ppBuffer, int& nWidth, int& nHeight, int& nBandNum, int& nBPB, size_t& nMemSize)
{
	if (ppBuffer)
	{
		delete[] ppBuffer;
		ppBuffer = nullptr;
	}

	nWidth = mSrc.cols;
	//nWidth = ((nWidth + 3) / 4) * 4;
	nHeight = mSrc.rows;
	nBandNum = mSrc.channels();
	nBPB = (mSrc.depth() >> 1) + 1;

	//size_t nMemSize = nWidth * nHeight * nBandNum * nBPB;				// {modified by yue.cui 2018/11/21
	nMemSize = nWidth * nHeight * nBandNum * nBPB;						// }end modify 2018/11/21
	//ÕâÑù¿ÉÒÔ¸Ä±äÍâ²¿*pBufferµÄÖµ
	ppBuffer = new uchar[nMemSize];
	memset(ppBuffer, 0, nMemSize);
	uchar* pT = ppBuffer;
	for (int j = 0; j < nHeight; ++j)
	{
		unsigned char* data = mSrc.ptr<unsigned char>(j);
		unsigned char* pSubBuffer = ppBuffer + (j)* nWidth * nBandNum * nBPB;
		memcpy(pSubBuffer, data, nWidth * nBandNum * nBPB);
	}

	return 0;
}

void myGaussian(const Mat _src, Mat & _dst)
{
	if (!_src.data) return;
	double **arr;
	Mat tmp(_src.size(), _src.type());
	for (int i = 0; i < _src.rows; ++i)
	{
		for (int j = 0; j < _src.cols; ++j) 
		{
			//边缘不进行处理
			if ((i - 1) > 0 && (i + 1) < _src.rows && (j - 1) > 0 && (j + 1) < _src.cols)
			{
				arr = getGuassionArray(3, 1);//自定义得到的权值数组
				tmp.at<Vec3b>(i, j)[0] = 0;
				tmp.at<Vec3b>(i, j)[1] = 0;
				tmp.at<Vec3b>(i, j)[2] = 0;
				for (int x = 0; x < 3; ++x)
				{
					for (int y = 0; y < 3; ++y)
					{
						tmp.at<Vec3b>(i, j)[0] += arr[x][y] * _src.at<Vec3b>(i + 1 - x, j + 1 - y)[0];
						tmp.at<Vec3b>(i, j)[1] += arr[x][y] * _src.at<Vec3b>(i + 1 - x, j + 1 - y)[1];
						tmp.at<Vec3b>(i, j)[2] += arr[x][y] * _src.at<Vec3b>(i + 1 - x, j + 1 - y)[2];
					}
				}
			}
		}
	}
	tmp.copyTo(_dst);
}

double ** getGuassionArray(int size, double sigma)
{
	int i, j;
	double sum = 0.0;
	int center = size; //以第一个点的坐标为原点，求出中心点的坐标
	double **arr = new double*[size];//建立一个size*size大小的二维数组
	for (i = 0; i < size; ++i)
		arr[i] = new double[size];
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j) {
			arr[i][j] = exp(-((i - center)*(i - center) + (j - center)*(j - center)) / (sigma*sigma * 2));
			sum += arr[i][j];
		}
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			arr[i][j] /= sum;
	return arr;
}

int myKmeans(Mat pic, int K)
{
	//Mat pic = imread("d:/woman.png");
	const int MAX_CLUSTERS = 6;
	Vec3b colorTab[] =
	{
		Vec3b(255, 0, 0),
		Vec3b(0, 255, 0),
		Vec3b(0, 0, 255),
		Vec3b(0, 255, 255),
		Vec3b(255, 0, 255),
		Vec3b(255, 255, 0),
		Vec3b(255, 100, 100),
	};
	Mat data, labels;
	for (int i = 0; i < pic.rows; i++)
		for (int j = 0; j < pic.cols; j++)
		{
			Vec3b point = pic.at<Vec3b>(i, j);
			Mat tmp = (Mat_<float>(1, 3) << point[0], point[1], point[2]);
			data.push_back(tmp);
		}

	//根据浏览图片，确定k
	kmeans(data, K, labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
		3, KMEANS_RANDOM_CENTERS);

	int n = 0;
	//显示聚类结果，不同的类别用不同的颜色显示
	for (int i = 0; i < pic.rows; i++)
		for (int j = 0; j < pic.cols; j++)
		{
			int clusterIdx = labels.at<int>(n);
			pic.at<Vec3b>(i, j) = colorTab[clusterIdx];
			n++;
		}
	//imshow("pic", pic);
	//waitKey(0);

	return 0;
}


int visulizeHist(Mat srcImage)
{
	//0表示将图像转换为灰度再返回
	//Mat srcImage = imread("dog.bmp", 0);
	//imshow("原图", srcImage);
	if (!srcImage.data) { cout << "fail to load image" << endl; 	return 0; }

	//定义变量
	Mat dstHist;
	int dims = 1;
	float hranges[] = { 0, 256 };
	const float *ranges[] = { hranges };   // 这里需要为const类型
	int size = 256;
	int channels = 0;

	//计算图像的直方图
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstImage(size, size, CV_8U, Scalar(0));
	//获取最大值和最小值
	double minValue = 0.0;
	double maxValue = 0.0;
	minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);  //  在cv中用的是cvGetMinMaxHistValue

	//绘制出直方图
	//saturate_cast函数的作用即是：当运算完之后，结果为负，则转为0，结果超出255，则为255。
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);           //   注意hist中是float类型   
		//拉伸到0-max
		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		line(dstImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	namedWindow("xxx", 0);
	imshow("xxx", dstImage);
	return 0;
}

int countBlocks(Mat mSrc, COLORBLOCK&  cb)
{
	int nCols = 0;
	int nRows = 0;
	nCols = mSrc.cols;
	//nWidth = ((nWidth + 3) / 4) * 4;
	nRows = mSrc.rows;
	Mat blockM(641, 1024, CV_8UC2, Scalar(0,0,0));
	for (int row = 0; row < nRows; ++row)
	{
		for (int col = 0; col < nCols; ++col)
		{
			if(cb.color == mSrc.ptr<Vec3b>(row)[col])
			{ 
				cb.setTrueAt(row, col);
				blockM.ptr<Vec3b>(row)[col] = Vec3b(0,0, 255);
			}
		}
	}
	imshow("BlockM", blockM);
	return 0;
}

int countBlocks(Mat mSrc, vector<COLORBLOCK>& vcb, Mat& blockM)
{
	ColorBlock cb;
	int nCols = 0;
	int nRows = 0;
	nCols = mSrc.cols;
	nRows = mSrc.rows;
	//Mat blockM(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	vector<ColorBlock>::iterator iter;
	Vec3b newColor;
	for (int row = 0; row < nRows; ++row)
	{
		for (int col = 0; col < nCols; ++col)
		{
			for (iter = vcb.begin(); iter != vcb.end(); ++iter)	//如果像素颜色已经记录过，则对应block进行标记
			{
				if (almostSame(iter->color, mSrc.ptr<Vec3b>(row)[col]))
				{
					iter->setTrueAt(row, col);
					blockM.ptr<Vec3b>(row)[col] = iter->color;
					break;
				}
			}
			if (iter == vcb.end())				//是一种新颜色，压入向量
			{
				cb.color = mSrc.ptr<Vec3b>(row)[col];
				cb.setTrueAt(row, col);
				vcb.push_back(cb);
			}
		}
	}
	//imshow("BlockM", blockM);
	int nCount = 0;
	for (iter = vcb.begin(); iter != vcb.end(); ++iter)	//如果像素颜色已经记录过，则对应block进行标记
	{
		nCount += iter->getCount();
	}
	return 0;
}

int countBlocksEx(Mat mSrc, vector<COLORBLOCK>& vcb, Mat & blockM)
{
	ColorBlock cb;
	int nCols = 0;
	int nRows = 0;
	nCols = mSrc.cols;
	nRows = mSrc.rows;
	//Mat blockM(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	vector<ColorBlock>::iterator iter;
	Vec3b newColor;
	for (int row = 0; row < nRows; ++row)
	{
		for (int col = 0; col < nCols; ++col)
		{
			for (iter = vcb.begin(); iter != vcb.end(); ++iter)	//如果像素颜色已经记录过，则对应block进行标记
			{
				if (almostSameEx(iter->color, mSrc.ptr<Vec3b>(row)[col]))
				{
					iter->setTrueAt(row, col);
					blockM.ptr<Vec3b>(row)[col] = iter->color;
					break;
				}
			}
			if (iter == vcb.end())				//是一种新颜色，压入向量
			{
				cb.color = mSrc.ptr<Vec3b>(row)[col];
				cb.setTrueAt(row, col);
				vcb.push_back(cb);
			}
		}
	}
	//imshow("BlockM", blockM);
	int nCount = 0;
	for (iter = vcb.begin(); iter != vcb.end(); ++iter)	//如果像素颜色已经记录过，则对应block进行标记
	{
		nCount += iter->getCount();
	}
	return 0;
}

int drawBlock(ColorBlock cb, int index)
{
	int nCols = 0;
	int nRows = 0;
	Mat blockM(641, 1024, CV_8UC3, Scalar(0, 0, 0));				//用于测试，显示找到颜色块
	for (int row = 0; row < 641; ++row)
	{
		for (int col = 0; col < 1024; ++col)
		{
			if (true == cb.getAt(row, col))
			{
				blockM.ptr<Vec3b>(row)[col] = cb.color;
			}
		}
	}
	char name[32] = {0};
	sprintf_s(name, 32, "%d", index);
	imshow(name, blockM);
	return 0;
}

int countVotes(Mat srcImage, COLORBLOCK& cb, ColorCandidate * cc)
{
	int b = 0;
	int g = 0;
	int r = 0;
	int y = 0;
	cc[0].nVotes = 0;
	cc[1].nVotes = 0;
	cc[2].nVotes = 0;
	cc[3].nVotes = 0;
	for (int i = 0; i < srcImage.rows; ++i)
	{
		for (int j = 0; j < srcImage.cols; ++j)
		{
			if (cb.getAt(i,j) == true)				//遍历指定区域，统计候选人票数
			{
				Vec3b color = srcImage.ptr<Vec3b>(i)[j];
				if (srcImage.ptr<Vec3b>(i)[j] == cc[0].color)		//蓝
				{
					cc[0].nVotes++;
					++b;
				}
				else if (srcImage.ptr<Vec3b>(i)[j] == cc[1].color)	//绿
				{
					cc[1].nVotes++;
					++g;
				}
				else if (srcImage.ptr<Vec3b>(i)[j] == cc[2].color)	//红
				{
					cc[2].nVotes++;
					++r;
				}
				else												//黄
				{
					cc[3].nVotes++;
					++y;
				}
			}
		}
	}
	return 0;
}

int eatAll(Mat& Msrc, COLORBLOCK& cb, ColorCandidate * cc)
{
	ColorCandidate winner = cc[0];
	int totalVotes = 0;
	for (int i = 0; i < 4; ++i)					//找到票数最多的胜利者
	{
		if (cc[i].nVotes > winner.nVotes)
		{
			winner = cc[i];
		}
		totalVotes += cc[i].nVotes;
	}
	float percent = float(winner.nVotes)/totalVotes;
	if (percent < 0.6)			//如果得票数过低则无效，不做处理
	{
		return 0;
	}
	for (int i = 0; i < Msrc.rows; ++i)
	{
		for (int j = 0; j < Msrc.cols; ++j)
		{
			if (cb.getAt(i,j) == true)				//把该区域全部标记为胜利者的颜色
			{
				Msrc.ptr<Vec3b>(i)[j] = winner.color;
			}
		}
	}
	return 0;
}

int almostSame(Vec3b ca, Vec3b cb, int range)
{
	if(abs(ca[0] - cb[0]) <= range)
	{
		return 1;
	}
	return 0;
}

int almostSameEx(Vec3b ca, Vec3b cb, float range)
{
	//计算(255 - abs(r1 - r2) * 0.297 - abs(g1 - g2) * 0.593 - abs(b1 - b2) * 0.11) / 255
	float fSimilarity = 0.0;
	fSimilarity = (255 - abs(ca[2] - cb[2]) * 0.297 - abs(ca[1] - cb[1]) * 0.593 - abs(ca[0] - cb[0]) * 0.11) / 255;
	if (range <= fSimilarity)
	{
		return 1;
	}
	return 0;
}

int forGroundTruth(Mat src, Vec3b bgc, Vec3b fc)
{
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			if(almostSameEx(src.ptr<Vec3b>(i)[j], bgc, 0.9))
			{
				src.ptr<Vec3b>(i)[j] = fc;
			}
		}
	}
	return 0;
}

int forGroundTruthEx(Mat src)
{
	Vec3b colorTab[4] = {
		Vec3b(255, 0, 0),
		Vec3b(0, 255, 0),
		Vec3b(0, 0, 255),
		Vec3b(0, 255, 255)
	};

	Vec3b vcTmp;
	int dist[4] = { 0 };
	int minIdx = 0;
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			vcTmp = src.ptr<Vec3b>(i)[j];
			dist[0] = abs(vcTmp[0] - colorTab[0][0]) + abs(vcTmp[1] - colorTab[0][1]) + abs(vcTmp[2] - colorTab[0][2]);
			dist[1] = abs(vcTmp[0] - colorTab[1][0]) + abs(vcTmp[1] - colorTab[1][1]) + abs(vcTmp[2] - colorTab[1][2]);
			dist[2] = abs(vcTmp[0] - colorTab[2][0]) + abs(vcTmp[1] - colorTab[2][1]) + abs(vcTmp[2] - colorTab[2][2]);
			dist[3] = abs(vcTmp[0] - colorTab[3][0]) + abs(vcTmp[1] - colorTab[3][1]) + abs(vcTmp[2] - colorTab[3][2]);
			minIdx =0;
			for (int k = 1; k < 4; ++k)
			{
				if (dist[minIdx] > dist[k])
				{
					minIdx = k;
				}
			}
			src.ptr<Vec3b>(i)[j] = colorTab[minIdx];
		}
	}

	return 0;
}

int forGroundTruth(Mat src, Mat dst, Vec3b bgc, Vec3b fc)
{
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			if (!almostSame(src.ptr<Vec3b>(i)[j], bgc, 30))				
			{
				dst.ptr<Vec3b>(i)[j] = fc;
			}
		}
	}
	return 0;
}

int serialize(Mat src, char* filename)
{
	int nTmp = 0;
	uchar cTmp = 0;
	Vec3b vecTmp = Vec3b(0, 0, 0);
	if (-1 == remove(filename));
	ofstream ofs(filename, ios::out | ios::binary);//打开record.txt文件，以ios::app追加的方式输入
	if (!ofs)
	{
		cout << "打开文件失败" << endl;
	}
	else
	{
		nTmp = src.rows;
		ofs.write((char*)&nTmp, sizeof(int));
		nTmp = src.cols;
		ofs.write((char*)&nTmp, sizeof(int));
		ofs.write((char*)src.data, sizeof(Vec3b)*src.rows*src.cols);
	}
	ofs.close();//关闭文件
	return 0;
}
int deserialize(Mat& dst, char* filename)
{
	int nTmp = 0;
	uchar cTmp = 0;
	Vec3b vecTmp = Vec3b(0, 0, 0);
	ifstream ifs(filename, ios::in|ios::binary);
	if (!ifs)
	{
		cout << "打开文件失败" << endl;
	}
	else
	{
		ifs.read((char*)&nTmp, sizeof(int));
		dst.rows = nTmp;
		ifs.read((char*)&nTmp, sizeof(int));
		dst.cols = nTmp;
		ifs.read((char*)dst.data, sizeof(Vec3b)*dst.rows*dst.cols);
		//ifs >> vecTmp[2];
		//dst.ptr<Vec3b>(i)[j] = vecTmp;
	}
	ifs.close();//关闭文件
	return 0;
}



int myMeanShiftFiltering(Mat img, Mat& res)
{
	//Mat img = imread(argv[1]); //读入图像，RGB三通道  
	//imshow("原图像", img);
	//Mat res; //分割后图像
	int spatialRad = 50;  //空间窗口大小
	int colorRad = 50;   //色彩窗口大小
	int maxPyrLevel = 2;  //金字塔层数
	pyrMeanShiftFiltering(img, res, spatialRad, colorRad, maxPyrLevel); //色彩聚类平滑滤波
	//imshow("res", res);
	RNG rng = theRNG();
	Mat mask(res.rows + 2, res.cols + 2, CV_8UC1, Scalar::all(0));  //掩模
	for (int y = 0; y < res.rows; y++)
	{
		for (int x = 0; x < res.cols; x++)
		{
			if (mask.at<uchar>(y + 1, x + 1) == 0)  //非0处即为1，表示已经经过填充，不再处理
			{
				Scalar newVal(rng(256), rng(256), rng(256));
				floodFill(res, mask, Point(x, y), newVal, 0, Scalar::all(5), Scalar::all(5)); //执行漫水填充
			}
		}
	}
	//imshow("meanShift图像分割", res);
	//waitKey();
	return 0;
}

CGetSpecificTime::CGetSpecificTime()
{
	QueryPerformanceFrequency(&nFreq);
}

CGetSpecificTime::~CGetSpecificTime()
{
}

void CGetSpecificTime::start()
{
	QueryPerformanceCounter(&nBeginTime);
}

void CGetSpecificTime::end()
{
	QueryPerformanceCounter(&nEndTime);
}

double CGetSpecificTime::GetSpecificTime()
{
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	return time;
}

double CGetSpecificTime::GetTime()
{
	return time;
}
