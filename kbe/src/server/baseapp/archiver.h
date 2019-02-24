// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ARCHIVER_H
#define KBE_ARCHIVER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
// #define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace KBEngine{

// 每过periodInTicks，一段时间，遍历所有，设置了自动保存的entity
// 将要保存的所有entity，分割成periodInTicks段
// 并且在periodInTicks间的每个tick，保存选取的那段entity到数据库

class Archiver
{
public:
	Archiver();
	~Archiver();
	
	void tick();
	void createArchiveTable();
	void archive(Entity& entity);

private:
	int						archiveIndex_;
	std::vector<ENTITY_ID> 	arEntityIDs_;
};


}

#endif // KBE_ARCHIVER_H
