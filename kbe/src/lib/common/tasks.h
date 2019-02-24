// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_TASKS_H
#define KBE_TASKS_H

#include "common/task.h"
#include "common/common.h"

namespace KBEngine
{

/**
 *	任务容器
 */

// 就是使用vector存储任务
// 批量调用process()
class Tasks
{
public:
	Tasks();
	~Tasks();

	// 添加task到vector
	void add(Task * pTask);

	// 从vector中移除task
	bool cancel(Task * pTask);

	// 遍历所有task，调用process()
	void process();

private:
	
	typedef std::vector<KBEngine::Task *> Container;
	Container container_;
};

}

#endif // KBE_TASKS_H
