// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#include "tasks.h"
#include "helper/debug_helper.h"
#include "thread/threadguard.h"

namespace KBEngine
{

Tasks::Tasks() : 
	container_()
{
}

//-------------------------------------------------------------------------------------
Tasks::~Tasks()
{
}

//-------------------------------------------------------------------------------------

// 添加task到vector
void Tasks::add( Task * pTask )
{
	container_.push_back( pTask );
}

//-------------------------------------------------------------------------------------

// 从vector中移除task
bool Tasks::cancel( Task * pTask )
{
	Container::iterator iter = std::find(container_.begin(), container_.end(), pTask);
	if (iter != container_.end())
	{
		container_.erase( iter );
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------

// 遍历所有task，调用process()
void Tasks::process()
{
	Container::iterator iter = container_.begin();

	while (iter != container_.end())
	{
		Task * pTask = *iter;
		if(!pTask->process())
			iter = container_.erase(iter);
		else
			++iter;
	}
}

//-------------------------------------------------------------------------------------
} 
