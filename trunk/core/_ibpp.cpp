///////////////////////////////////////////////////////////////////////////////
//
//	File    : $Id$
//	Subject : IBPP, Initialization of the library
//
///////////////////////////////////////////////////////////////////////////////
//
//	(C) Copyright 2000-2006 T.I.P. Group S.A. and the IBPP Team (www.ibpp.org)
//
//	The contents of this file are subject to the IBPP License (the "License");
//	you may not use this file except in compliance with the License.  You may
//	obtain a copy of the License at http://www.ibpp.org or in the 'license.txt'
//	file which must have been distributed along with this file.
//
//	This software, distributed under the License, is distributed on an "AS IS"
//	basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
//	License for the specific language governing rights and limitations
//	under the License.
//
///////////////////////////////////////////////////////////////////////////////
//
//	COMMENTS
//	* Tabulations should be set every four characters when editing this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786 4996)
#ifndef _DEBUG
#pragma warning(disable: 4702)
#endif
#endif

#include "_ibpp.h"

#ifdef HAS_HDRSTOP
#pragma hdrstop
#endif

#include <limits>

namespace ibpp_internals
{
	const double consts::dscales[19] = {
		  1, 1E1, 1E2, 1E3, 1E4, 1E5, 1E6, 1E7, 1E8,
		  1E9, 1E10, 1E11, 1E12, 1E13, 1E14, 1E15,
		  1E16, 1E17, 1E18 };

	const int consts::Dec31_1899 = 693595;

// Many compilers confuses those following min/max with macros min and max !
#undef min
#undef max

#ifdef __DMC__ // Needs to break-down the declaration else compiler crash (!)
	const std::numeric_limits<int16_t> i16_limits;
	const std::numeric_limits<int32_t> i32_limits;
	const int16_t consts::min16 = i16_limits.min();
	const int16_t consts::max16 = i16_limits.max();
	const int32_t consts::min32 = i32_limits.min();
	const int32_t consts::max32 = i32_limits.max();
#else
	const int16_t consts::min16 = std::numeric_limits<int16_t>::min();
	const int16_t consts::max16 = std::numeric_limits<int16_t>::max();
	const int32_t consts::min32 = std::numeric_limits<int32_t>::min();
	const int32_t consts::max32 = std::numeric_limits<int32_t>::max();
#endif

#ifdef _DEBUG
	std::ostream& operator<< (std::ostream& a, flush_debug_stream_type)
	{
		if (std::stringstream* p = dynamic_cast<std::stringstream*>(&a))
		{
#ifdef IBPP_WINDOWS
			::OutputDebugString(("IBPP: " + p->str() + "\n").c_str());
#endif
			p->str("");
		}
		return a;
	}
#endif	// _DEBUG

}

using namespace ibpp_internals;

namespace IBPP
{

	bool CheckVersion(uint32_t AppVersion)
	{
		//(void)gds.Call(); 		// Just call it to trigger the initialization
		return (AppVersion & 0xFFFFFF00) ==
				(IBPP::Version & 0xFFFFFF00) ? true : false;
	}

	int GDSVersion()
	{
		//return gds.Call()->mGDSVersion;
		return 60;
	}

	Driver DriverFactory()
	{
		return new DriverImpl();
	}

}

//
//	EOF
//

