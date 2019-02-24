// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_BACKUPER_H
#define KBE_BACKUPER_H

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

// 同archiver
// 一段间隔内，备份所有设置了自动备份功能的entity
// 每一个tick，只备份一段
class Backuper
{
public:
	Backuper();
	~Backuper();
	
	void tick();

	void createBackupTable();

	bool backup(Entity& entity, MemoryStream& s);

private:
	// 在此列表中的entity将进行备份操作
	std::vector<ENTITY_ID>		backupEntityIDs_;

	float						backupRemainder_;
};


}

#endif // KBE_BACKUPER_H
