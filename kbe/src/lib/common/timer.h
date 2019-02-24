// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com
#ifndef KBE_TIMER_H
#define KBE_TIMER_H

#include "common/common.h"
#include "common/timestamp.h"
#include "helper/debug_helper.h"

namespace KBEngine
{
class TimersBase;
class TimeBase;

// Q: 用来调用Timer的cancel()函数的？
// 用来引用Timer的，所以叫做Timer Handle ?
class TimerHandle
{
public:
	explicit TimerHandle(TimeBase * pTime = NULL) : pTime_( pTime ) {}

	// 调用TimeBase pTime_的cancel()
	// 并置空pTime_
	void cancel();

	// 置空pTime_
	// 但是不调用pTime_的Cancel()
	void clearWithoutCancel()	{ pTime_ = NULL; }

	// 是否有设置pTime
	bool isSet() const		{ return pTime_ != NULL; }

	// 比较2个TimerHandle的pTime_是否相同
	friend bool operator==( TimerHandle h1, TimerHandle h2 );

	// 获取pTime
	TimeBase * time() const	{ return pTime_; }

private:
	TimeBase * pTime_;
};

inline bool operator==( TimerHandle h1, TimerHandle h2 )
{
	return h1.pTime_ == h2.pTime_;
}


/**
 *	必须继承这个接口
 *	来接收timer->handleTimeout事件
 */

// * 这个应该是Timer的回调处理
// * 需要自定义类，来继承此类的
// * 被Timer所使用，应该是被Timer来调用
// * 有计数，被传入几个Timer中
class TimerHandler
{
public:
	TimerHandler() : numTimesRegistered_( 0 ) {}

	virtual ~TimerHandler()
	{
		KBE_ASSERT( numTimesRegistered_ == 0 );
	};

	// Q: 处理Timer到时?
	virtual void handleTimeout(TimerHandle handle, void * pUser) = 0;

protected:
	// 从Timer中被解除时候的回调？
	virtual void onRelease( TimerHandle handle, void * pUser ) {
	}

private:
	friend class TimeBase;

	// 被传入Timer构造函数，被Timer使用时候，进行计数
	void incTimerRegisterCount() { ++numTimesRegistered_; }
	void decTimerRegisterCount() { --numTimesRegistered_; }

	// Timer进行Release()的时候，会回调在这里
	// 解除同Timer的绑定
	void release( TimerHandle handle, void * pUser )
	{
		this->decTimerRegisterCount();
		this->onRelease( handle, pUser );
	}

	// 被传入几个Timer中
	int numTimesRegistered_;
};

// 应该叫做TimerBase
// 所有者TimersBase
// 有自己的状态pending, executing, cancelled
// 有TimerHandler
//		* Timer到时回调？
class TimeBase
{
public:

	// 在timer.inc中实现
	TimeBase(TimersBase &owner, TimerHandler* pHandler, 
		void* pUserData);
	
	virtual ~TimeBase(){}

	// 进行Cancel
	// 设置状态CANCELLED
	// 通知TimerHandler，Timer被Cancel
	// 通知Timers，Timer被Cancel
	void cancel();

	// 获取绑定的自定义数据
	void * getUserData() const	{ return pUserData_; }

	// 判断自己的状态
	bool isCancelled() const{ return state_ == TIME_CANCELLED; }
	bool isExecuting() const{ return state_ == TIME_EXECUTING; }

protected:

	// Timer状态
	enum TimeState
	{
		TIME_PENDING,
		TIME_EXECUTING,
		TIME_CANCELLED
	};

	// Timer的拥有者/管理者
	TimersBase& owner_;

	// 此Timer事件的响应
	TimerHandler * pHandler_;

	// 绑定的用户自定义数据
	void *pUserData_;

	// Timer状态
	TimeState state_;
};

// 只定义了个onCancel
class TimersBase
{
public:
	virtual void onCancel() = 0;
};

// 使用vector，拥有多个TimeBase
// vector又会被PriorityQueue给组织成heap，进行排序?
// 模板类TimersT，被实例化了2个类 Timers和 Timers64
// 也就是
//	+ TimersBase
//		+ TimersT
//			+ Timers
//			+ Timers64
// 定义了内部类Time:TimeBase, PriorityQueue
template<class TIME_STAMP>
class TimersT : public TimersBase
{
public:
	typedef TIME_STAMP TimeStamp;

	TimersT();
	virtual ~TimersT();
	
	// 获取timeQueue大小
	inline uint32 size() const	{ return timeQueue_.size(); }

	// 判断timeQueue是否空
	inline bool empty() const	{ return timeQueue_.empty(); }
	
	int	process(TimeStamp now);

	bool legal( TimerHandle handle ) const;

	TIME_STAMP nextExp( TimeStamp now ) const;
	void clear( bool shouldCallCancel = true );
	
	bool getTimerInfo( TimerHandle handle, 
					TimeStamp& time, 
					TimeStamp&	interval,
					void *&	pUser ) const;
	
	TimerHandle	add(TimeStamp startTime, TimeStamp interval,
						TimerHandler* pHandler, void * pUser);
	
private:
	
	// 拥有的TimeBase
	// Q: contianer_这个变量没有被使用？
	// PriorityQueue中，已经有了自己的Container
	typedef std::vector<KBEngine::TimeBase *> Container;
	Container container_;

	void purgeCancelledTimes();
	void onCancel();

	// TimeBase:
	// 		* 所有者TimersBase
	// 		* 有自己的状态pending, executing, cancelled
	// 		* 有TimerHandler
	// 		* Q: 这个TimeBase，是不是应该叫做TimerBase比较好点
	// Time: 
	//		* 叫Timer是不是比较好点
	//		* 增加time, interval

	class Time : public TimeBase
	{
	public:
		Time( TimersBase & owner, TimeStamp startTime, TimeStamp interval,
			TimerHandler * pHandler, void * pUser );

		// Q: 这个是开始时间？
		TIME_STAMP time() const			{ return time_; }
		TIME_STAMP interval() const		{ return interval_; }

		void triggerTimer();

	private:
		// Q: 开始时间？
		TimeStamp			time_;
		TimeStamp			interval_;

		Time( const Time & );
		Time & operator=( const Time & );
	};

	// 比较Timer的time (开始时间？)
	// 用于PriorityQueue中Container被当做heap来使用
	class Comparator
	{
	public:
		bool operator()(const Time* a, const Time* b)
		{
			return a->time() > b->time();
		}
	};
	
	// 使用堆排序，来排序Time
	// T: 排序算法，都有自己的优点缺点.
	// 这些在底层系统中的功能，而且在服务器处理大量批量的数据
	// 就需要考虑各个算法的优劣来进行选取
	// 而不能像lua那样，为了简洁，所有的数据结构都是table，对于初学者来说，不需要了解数据结构知识！
	class PriorityQueue
	{
	public:
		// Time: TimeBase
		// Container被当做heap来使用
		typedef std::vector<Time *> Container;

		// value_type: Container的valueType
		typedef typename Container::value_type value_type;
		// size_type: Container的sizeType
		typedef typename Container::size_type size_type;

		// Container是否空
		bool empty() const				{ return container_.empty(); }

		// 获取Container中Time个数
		size_type size() const			{ return container_.size(); }

		// 获取Container中，第一个值
		const value_type & top() const	{ return container_.front(); }

		// 加入Time到Container
		void push( const value_type & x )
		{
			container_.push_back( x );
			// Container被当做heap来使用
			// 最末尾加入container的元素，将会被放入到heap中合理的位置上去
			std::push_heap( container_.begin(), container_.end(),
					Comparator() );
		}

		// 从堆中取出一个最大的元素
		void pop()
		{
			// 从堆中取出一个元素
			// 最大的元素，会被移动到末尾
			std::pop_heap( container_.begin(), container_.end(), Comparator() );
			container_.pop_back();
		}

		// 非安全的pop()
		// （没有考虑堆是否有在维护）
		Time * unsafePopBack()
		{
			Time * pTime = container_.back();
			container_.pop_back();
			return pTime;
		}

		Container & container()		{ return container_; }

		// 将Container构建成heap
		void make_heap()
		{
			std::make_heap( container_.begin(), container_.end(),
					Comparator() );
		}

	private:
		Container container_;
	};
	
	PriorityQueue	timeQueue_;
	//	当前在处理的Timer?
	Time * 			pProcessingNode_;	
	TimeStamp 		lastProcessTime_;
	int				numCancelled_;

	TimersT( const TimersT & );
	TimersT & operator=( const TimersT & );

};	// end TimersT

// 模板TimersT的2个类型实例
typedef TimersT<uint32> Timers;
typedef TimersT<uint64> Timers64;
}

// Q: ++ *.inl文件
//	* inline file ?
//	* 为什么用一个新的概念inl，为什么不直接用cpp
//	* 可以定义inline函数
//	* 可以定义函数模板

#include "timer.inl"

#endif // KBE_TIMER_H
