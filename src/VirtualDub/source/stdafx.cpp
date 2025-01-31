#include <stdafx.h>

#ifdef _MSC_VER
	#pragma hdrstop
#endif

// compiler/setup checks

#if defined(_MSC_VER)
	#if _MSC_VER < 1400
		#error MSVC 14.00 (VC 8.0) compiler or newer required. 
	#endif
#endif
