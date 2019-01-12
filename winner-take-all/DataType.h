#pragma once
#include "pch.h"
#include "CPersistent.h"
using namespace persistent;
//////////////////////CommonDataType////////////////////////////////////////
#pragma region CommonDataType
typedef struct myPOINT
{
	int  x;
	int  y;
	myPOINT() :x(0), y(0) {};
	myPOINT(int X, int Y) :x(X), y(Y) {  };
} MYPOINT, *PMYPOINT;

typedef struct ColorBlock
{
	Vec3b color;
	byte block[128 * 641] = { 0 };			//һ����8*128*641��Ҳ����1024*641λ������ֱ���Ϊ1024*641��ͼƬ
	int nCount;
	int nLen = 128 * 641;
	ColorBlock():color(0,0,0),nCount(0)
	{

	};
	ColorBlock(Vec3b color):nCount(0)
	{ 
		this->color = color;
	};
	void operator=(ColorBlock& cb)
	{
		if (this != &cb)
		{
			this->color = cb.color;
			memcpy(this->block, cb.block, nLen);
			this->nCount = cb.nCount;
			this->nLen = cb.nLen;
		}
	};
	void setTrueAt(int i, int j)
	{
		int pos = i * 1024 + j;			//�����i,j������ǵڼ�������
		i = pos / 8;					//�����������block�еĵڼ���byte	
		j = pos % 8;					//�����������block�е�ĳ��byte�ĵڼ�λ
		uchar mask = 0x80 >> j;	//ʹָ��λΪ0
		block[i] |= mask;			//��ָ��λ��Ϊ1
	};
	void setFalseAt(int i, int j)
	{
		int pos = i * 1024 + j;			//�����i,j������ǵڼ�������
		i = pos / 8;					//�����������block�еĵڼ���byte	
		j = pos % 8;					//�����������block�е�ĳ��byte�ĵڼ�λ
		uchar mask = 0XFF - 0x80 >> j;	//ʹָ��λΪ0
		block[i] &= mask;			//��ָ��λ��Ϊ1
	};
	/*
	@�Զ�ά���鷽ʽ����, ��λ����
	*/
	bool getAt(int i, int j)
	{
		int pos = i * 1024 + j;			//�����i,j������ǵڼ�������
		i = pos / 8;					//�����������block�еĵڼ���byte	
		j = pos % 8;					//�����������block�е�ĳ��byte�ĵڼ�λ
		uchar mask = 0x80 >> j;	//ʹָ��λΪ0
		if (0 == (mask & block[i]))
		{
			return false;
		}
		else
		{
			return true;
		}

	};
	/*
	@��һά���鷽ʽ����, ��byte��
	*/
	byte getAt(int i)
	{
		return block[i];
	};
	/*
	@��һά���鷽ʽ����, ��byteд
	*/
	void setAt(int i, byte bt)
	{
		block[i] = bt;
	};
	int getCount()
	{
		nCount = 0;
		for (int i = 0; i < nLen; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				nCount += 0x01 & (block[i] >> j);
			}
		}
		return nCount;
	};
} COLORBLOCK, *PCOLORBLOCK;

typedef struct ColorCandidate
{
	Vec3b color = Vec3b(0, 0, 0);
	int nVotes = 0;
	ColorCandidate() {};
	ColorCandidate(uchar b, uchar g, uchar r) { this->color = Vec3b(b, g, r); };
	ColorCandidate(Vec3b color) { this->color = color; };
} COLORCANDIDATE, *PCOLORCANDIDATE;

//test
struct mydata : public serializable
{
public:
	char _c;
	wchar_t _w;
	bool _b;
	short _s;
	long _l;
	int64 _64;
	double _d;
	std::string _str;
	std::wstring _wstr;
public:
	virtual void serialize(serializer& out) const
	{
		out.put_char(_c);
		out.put_wchar(_w);
		out.put_bool(_b);
		out.put_short(_s);
		out.put_long(_l);
		out.put_llong(_64);
		out.put_double(_d);
		out.put_string(_str);
		out.put_wstring(_wstr);
	}
	virtual void deserialize(deserializer& in)
	{
		_c = in.get_char();
		_w = in.get_wchar();
		_b = in.get_bool();
		_s = in.get_short();
		_l = in.get_long();
		_64 = in.get_llong();
		_d = in.get_double();
		_str = in.get_string();
		_wstr = in.get_wstring();
	}
	bool operator == (const mydata& r)
	{
		return _c == r._c &&
			_w == r._w &&
			_b == r._b &&
			_s == r._s &&
			_l == r._l &&
			_64 == r._64 &&
			_d == r._d &&
			_str == r._str &&
			_wstr == r._wstr;
	}
};
#pragma endregion


