// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_TASKS_H
#define KBE_TASKS_H

#include "common/task.h"
#include "common/common.h"

namespace KBEngine
{

/**
 *	��������
 */

// ����ʹ��vector�洢����
// ��������process()
class Tasks
{
public:
	Tasks();
	~Tasks();

	// ���task��vector
	void add(Task * pTask);

	// ��vector���Ƴ�task
	bool cancel(Task * pTask);

	// ��������task������process()
	void process();

private:
	
	typedef std::vector<KBEngine::Task *> Container;
	Container container_;
};

}

#endif // KBE_TASKS_H
