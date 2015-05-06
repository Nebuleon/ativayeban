/*
    Copyright (c) 2015, Cong Xu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <assert.h>
#include <stdio.h>

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#define CLAMP(v, _min, _max) MAX((_min), MIN((_max), (v)))
#define SIGN(_x) ((_x) < 0 ? -1 : 1)
#define UNUSED(expr) (void)(expr)

#ifdef _MSC_VER
#define CHALT() __debugbreak()
#else
#define CHALT()
#endif

#define CASSERT(_x, _errmsg)\
{\
	volatile bool isOk = _x;\
	if (!isOk)\
	{\
		static char _buf[1024];\
		sprintf(\
			_buf,\
			"In %s %d:%s: " _errmsg " (" #_x ")",\
			__FILE__, __LINE__, __func__);\
		CHALT();\
		assert(_x);\
	}\
}

#define _CCHECKALLOC(_func, _var, _size)\
{\
	if (_var == NULL && _size > 0)\
	{\
		exit(1);\
	}\
}

#define CMALLOC(_var, _size)\
{\
	_var = malloc(_size);\
	_CCHECKALLOC("CMALLOC", _var, (_size))\
}
#define CCALLOC(_var, _size)\
{\
	_var = calloc(1, _size);\
	_CCHECKALLOC("CCALLOC", _var, (_size))\
}
#define CREALLOC(_var, _size)\
{\
	_var = realloc(_var, _size);\
	_CCHECKALLOC("CREALLOC", _var, (_size))\
}
#define CSTRDUP(_var, _str)\
{\
	CMALLOC(_var, strlen(_str) + 1);\
	strcpy(_var, _str);\
}

#define CFREE(_var)\
{\
	free(_var);\
}