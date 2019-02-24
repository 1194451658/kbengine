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

	// 构造函数

	Address();
	Address(uint32 ipArg, uint16 portArg);

	// 字符串ip地址
	Address(std::string ipArg, uint16 portArg);
	Address(const Address& addr);

	virtual ~Address();

	// IP地址(网络字节序)
	uint32	ip;
	// 端口号
	uint16	port;

	// 获取点分字符串ip地址, X.X.X.X:port
	int writeToString(char * str, int length) const;

	// 重载类型转换操作符，char*
	operator char*() const { return this->c_str(); }

	// 获取点分字符串ip地址, X.X.X.X:port
	// (使用自己的s_stringBuf缓存)
	char * c_str() const;

	const char * ipAsString() const;
	bool isNone() const	{ return this->ip == 0; }

	static int string2ip(const char * string, u_int32_t & address);
	static int ip2string(u_int32_t address, char * string);

private:

	// 2个临时的字符串buff
	// （防止重复分配内存）
	static char s_stringBuf[2][32];

	// 当前指向的s_stringBuf
	static int s_currStringBuf;

	// 循环获取，下一个s_stringBuf
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

// 比较操作符重载
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
