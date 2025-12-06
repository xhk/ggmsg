%module(directors="1") ggmsg
%{ 
#include "../ggmsg/ChannelNode.h"
%}


%typemap(ctype) char *BYTE "jbyteArray"
%typemap(imtype) char *BYTE "byte[]"
%typemap(cstype) char *BYTE "byte[]"

%apply void *pData{ void *}

%feature("director") ChannelNode; 
%include "../ggmsg/ChannelNode.h"
