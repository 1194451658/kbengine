// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ADDRESS_H
#define KBE_ADDRESS_H

#include "common/common.h"
#include "common/objectpool.h"
#include "network/common.h"

namespace KBEngine { 
namespace Network
{
class Address  : public PoolObject
{
public:
	static const Address NONE;

	typedef KBEShared_ptr< SmartPoolObject< Address > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<Address>& ObjPool();
	static Address* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Address* obj);
	static void destroyObjPool();
	void onReclaimObject();

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(ip)
		 + sizeof(port);

		return bytes;
	}

	// ���캯��

	Address();
	Address(uint32 ipArg, uint16 portArg);

	// �ַ���ip��ַ
	Address(std::string ipArg, uint16 portArg);
	Address(const Address& addr);

	virtual ~Address();

	// IP��ַ(�����ֽ���)
	uint32	ip;
	// �˿ں�
	uint16	port;

	// ��ȡ����ַ���ip��ַ, X.X.X.X:port
	int writeToString(char * str, int length) const;

	// ��������ת����������char*
	operator char*() const { return this->c_str(); }

	// ��ȡ����ַ���ip��ַ, X.X.X.X:port
	// (ʹ���Լ���s_stringBuf����)
	char * c_str() const;

	const char * ipAsString() const;
	bool isNone() const	{ return this->ip == 0; }

	static int string2ip(const char * string, u_int32_t & address);
	static int ip2string(u_int32_t address, char * string);

private:

	// 2����ʱ���ַ���buff
	// ����ֹ�ظ������ڴ棩
	static char s_stringBuf[2][32];

	// ��ǰָ���s_stringBuf
	static int s_currStringBuf;

	// ѭ����ȡ����һ��s_stringBuf
	static char * nextStringBuf();
};

inline Address::Address():
ip(0),
port(0)
{
}

inline Address::Address(uint32 ipArg, uint16 portArg) :
	ip(ipArg),
	port(portArg)
{
} 

// �Ƚϲ���������
inline bool operator==(const Address & a, const Address & b)
{
	return (a.ip == b.ip) && (a.port == b.port);
}

inline bool operator!=(const Address & a, const Address & b)
{
	return (a.ip != b.ip) || (a.port != b.port);
}

inline bool operator<(const Address & a, const Address & b)
{
	return (a.ip < b.ip) || (a.ip == b.ip && (a.port < b.port));
}


}
}
#endif // KBE_ADDRESS_H
