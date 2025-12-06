#include "stdafx.h"
#include "ChannelNode.h"
#include "ChannelMgr.h"

ChannelNode* ChannelNode::Create(ChannelListener* pListener) {
	return new ChannelMgr(pListener);
}

void ChannelNode::Destory(ChannelNode* pNode)
{
	delete (ChannelMgr*)pNode;
}