%module(directors="1") jggmsg

%apply (char *STRING, size_t LENGTH) { (const char* pData, int nDataLen) }

%{ 
#include "../ggmsg/ChannelNode.h"
%}



%feature("director") ChannelListener; 
%include "../ggmsg/ChannelNode.h"