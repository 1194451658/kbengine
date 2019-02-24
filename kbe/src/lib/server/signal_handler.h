// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_SIGNAL_HANDLER_H
#define KBE_SIGNAL_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/singleton.h"

	
namespace KBEngine{
class ServerApp;

class SignalHandler
{
public:
	virtual void onSignalled(int sigNum) = 0;
};

// Task: 只定义一个process()虚函数
// 单例
// 用来监听从某个ServerApp发出的signal，缓存在signalledArray_中
// 在process()函数中，会对signalledArray_中的消息进行处理
// 注：ServerApp就继承自SignalHandler !!!
// Q: 对于一个signal，只有一个Handler?
class SignalHandlers : public Singleton<SignalHandlers>, public Task
{
public:
	SignalHandlers();
	~SignalHandlers();
	
	SignalHandler* addSignal(int sigNum, 
		SignalHandler* pSignalHandler, int flags = 0);
	
	SignalHandler* delSignal(int sigNum);
	
	bool ignoreSignal(int sigNum);

	void clear();
	
	void onSignalled(int sigNum);
	
	virtual bool process();

	// Q: 和App绑定？
	// 接收从App发送出来的signale?
	void attachApp(ServerApp* app);

	ServerApp* getApp(){ return papp_; }

private:
	typedef std::map<int, SignalHandler*> SignalHandlerMap;
	SignalHandlerMap singnalHandlerMap_;
	
	ServerApp* papp_;
	uint8 rpos_, wpos_;


	// onSignalled()的传入，会被缓存在这里
	// process()的时候，会遍历这里进行处理
	int signalledArray_[256];

};

#define g_kbeSignalHandlers SignalHandlers::getSingleton()
}
#endif // KBE_SIGNAL_HANDLER_H
