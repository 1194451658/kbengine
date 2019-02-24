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

// Q: ��������Timer��cancel()�����ģ�
// ��������Timer�ģ����Խ���Timer Handle ?
class TimerHandle
{
public:
	explicit TimerHandle(TimeBase * pTime = NULL) : pTime_( pTime ) {}

	// ����TimeBase pTime_��cancel()
	// ���ÿ�pTime_
	void cancel();

	// �ÿ�pTime_
	// ���ǲ�����pTime_��Cancel()
	void clearWithoutCancel()	{ pTime_ = NULL; }

	// �Ƿ�������pTime
	bool isSet() const		{ return pTime_ != NULL; }

	// �Ƚ�2��TimerHandle��pTime_�Ƿ���ͬ
	friend bool operator==( TimerHandle h1, TimerHandle h2 );

	// ��ȡpTime
	TimeBase * time() const	{ return pTime_; }

private:
	TimeBase * pTime_;
};

inline bool operator==( TimerHandle h1, TimerHandle h2 )
{
	return h1.pTime_ == h2.pTime_;
}


/**
 *	����̳�����ӿ�
 *	������timer->handleTimeout�¼�
 */

// * ���Ӧ����Timer�Ļص�����
// * ��Ҫ�Զ����࣬���̳д����
// * ��Timer��ʹ�ã�Ӧ���Ǳ�Timer������
// * �м����������뼸��Timer��
class TimerHandler
{
public:
	TimerHandler() : numTimesRegistered_( 0 ) {}

	virtual ~TimerHandler()
	{
		KBE_ASSERT( numTimesRegistered_ == 0 );
	};

	// Q: ����Timer��ʱ?
	virtual void handleTimeout(TimerHandle handle, void * pUser) = 0;

protected:
	// ��Timer�б����ʱ��Ļص���
	virtual void onRelease( TimerHandle handle, void * pUser ) {
	}

private:
	friend class TimeBase;

	// ������Timer���캯������Timerʹ��ʱ�򣬽��м���
	void incTimerRegisterCount() { ++numTimesRegistered_; }
	void decTimerRegisterCount() { --numTimesRegistered_; }

	// Timer����Release()��ʱ�򣬻�ص�������
	// ���ͬTimer�İ�
	void release( TimerHandle handle, void * pUser )
	{
		this->decTimerRegisterCount();
		this->onRelease( handle, pUser );
	}

	// �����뼸��Timer��
	int numTimesRegistered_;
};

// Ӧ�ý���TimerBase
// ������TimersBase
// ���Լ���״̬pending, executing, cancelled
// ��TimerHandler
//		* Timer��ʱ�ص���
class TimeBase
{
public:

	// ��timer.inc��ʵ��
	TimeBase(TimersBase &owner, TimerHandler* pHandler, 
		void* pUserData);
	
	virtual ~TimeBase(){}

	// ����Cancel
	// ����״̬CANCELLED
	// ֪ͨTimerHandler��Timer��Cancel
	// ֪ͨTimers��Timer��Cancel
	void cancel();

	// ��ȡ�󶨵��Զ�������
	void * getUserData() const	{ return pUserData_; }

	// �ж��Լ���״̬
	bool isCancelled() const{ return state_ == TIME_CANCELLED; }
	bool isExecuting() const{ return state_ == TIME_EXECUTING; }

protected:

	// Timer״̬
	enum TimeState
	{
		TIME_PENDING,
		TIME_EXECUTING,
		TIME_CANCELLED
	};

	// Timer��ӵ����/������
	TimersBase& owner_;

	// ��Timer�¼�����Ӧ
	TimerHandler * pHandler_;

	// �󶨵��û��Զ�������
	void *pUserData_;

	// Timer״̬
	TimeState state_;
};

// ֻ�����˸�onCancel
class TimersBase
{
public:
	virtual void onCancel() = 0;
};

// ʹ��vector��ӵ�ж��TimeBase
// vector�ֻᱻPriorityQueue����֯��heap����������?
// ģ����TimersT����ʵ������2���� Timers�� Timers64
// Ҳ����
//	+ TimersBase
//		+ TimersT
//			+ Timers
//			+ Timers64
// �������ڲ���Time:TimeBase, PriorityQueue
template<class TIME_STAMP>
class TimersT : public TimersBase
{
public:
	typedef TIME_STAMP TimeStamp;

	TimersT();
	virtual ~TimersT();
	
	// ��ȡtimeQueue��С
	inline uint32 size() const	{ return timeQueue_.size(); }

	// �ж�timeQueue�Ƿ��
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
	
	// ӵ�е�TimeBase
	// Q: contianer_�������û�б�ʹ�ã�
	// PriorityQueue�У��Ѿ������Լ���Container
	typedef std::vector<KBEngine::TimeBase *> Container;
	Container container_;

	void purgeCancelledTimes();
	void onCancel();

	// TimeBase:
	// 		* ������TimersBase
	// 		* ���Լ���״̬pending, executing, cancelled
	// 		* ��TimerHandler
	// 		* Q: ���TimeBase���ǲ���Ӧ�ý���TimerBase�ȽϺõ�
	// Time: 
	//		* ��Timer�ǲ��ǱȽϺõ�
	//		* ����time, interval

	class Time : public TimeBase
	{
	public:
		Time( TimersBase & owner, TimeStamp startTime, TimeStamp interval,
			TimerHandler * pHandler, void * pUser );

		// Q: ����ǿ�ʼʱ�䣿
		TIME_STAMP time() const			{ return time_; }
		TIME_STAMP interval() const		{ return interval_; }

		void triggerTimer();

	private:
		// Q: ��ʼʱ�䣿
		TimeStamp			time_;
		TimeStamp			interval_;

		Time( const Time & );
		Time & operator=( const Time & );
	};

	// �Ƚ�Timer��time (��ʼʱ�䣿)
	// ����PriorityQueue��Container������heap��ʹ��
	class Comparator
	{
	public:
		bool operator()(const Time* a, const Time* b)
		{
			return a->time() > b->time();
		}
	};
	
	// ʹ�ö�����������Time
	// T: �����㷨�������Լ����ŵ�ȱ��.
	// ��Щ�ڵײ�ϵͳ�еĹ��ܣ������ڷ����������������������
	// ����Ҫ���Ǹ����㷨������������ѡȡ
	// ��������lua������Ϊ�˼�࣬���е����ݽṹ����table�����ڳ�ѧ����˵������Ҫ�˽����ݽṹ֪ʶ��
	class PriorityQueue
	{
	public:
		// Time: TimeBase
		// Container������heap��ʹ��
		typedef std::vector<Time *> Container;

		// value_type: Container��valueType
		typedef typename Container::value_type value_type;
		// size_type: Container��sizeType
		typedef typename Container::size_type size_type;

		// Container�Ƿ��
		bool empty() const				{ return container_.empty(); }

		// ��ȡContainer��Time����
		size_type size() const			{ return container_.size(); }

		// ��ȡContainer�У���һ��ֵ
		const value_type & top() const	{ return container_.front(); }

		// ����Time��Container
		void push( const value_type & x )
		{
			container_.push_back( x );
			// Container������heap��ʹ��
			// ��ĩβ����container��Ԫ�أ����ᱻ���뵽heap�к����λ����ȥ
			std::push_heap( container_.begin(), container_.end(),
					Comparator() );
		}

		// �Ӷ���ȡ��һ������Ԫ��
		void pop()
		{
			// �Ӷ���ȡ��һ��Ԫ��
			// ����Ԫ�أ��ᱻ�ƶ���ĩβ
			std::pop_heap( container_.begin(), container_.end(), Comparator() );
			container_.pop_back();
		}

		// �ǰ�ȫ��pop()
		// ��û�п��Ƕ��Ƿ�����ά����
		Time * unsafePopBack()
		{
			Time * pTime = container_.back();
			container_.pop_back();
			return pTime;
		}

		Container & container()		{ return container_; }

		// ��Container������heap
		void make_heap()
		{
			std::make_heap( container_.begin(), container_.end(),
					Comparator() );
		}

	private:
		Container container_;
	};
	
	PriorityQueue	timeQueue_;
	//	��ǰ�ڴ����Timer?
	Time * 			pProcessingNode_;	
	TimeStamp 		lastProcessTime_;
	int				numCancelled_;

	TimersT( const TimersT & );
	TimersT & operator=( const TimersT & );

};	// end TimersT

// ģ��TimersT��2������ʵ��
typedef TimersT<uint32> Timers;
typedef TimersT<uint64> Timers64;
}

// Q: ++ *.inl�ļ�
//	* inline file ?
//	* Ϊʲô��һ���µĸ���inl��Ϊʲô��ֱ����cpp
//	* ���Զ���inline����
//	* ���Զ��庯��ģ��

#include "timer.inl"

#endif // KBE_TIMER_H
