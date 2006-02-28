///////////////////////////////////////////////////////////////////////////////
//
//	File    : $Id$
//	Subject : IBPP, Date class implementation
//
///////////////////////////////////////////////////////////////////////////////
//
//	The contents of this file are subject to the Mozilla Public License
//	Version 1.0 (the "License"); you may not use this file except in
//	compliance with the License. You may obtain a copy of the License at
//	http://www.mozilla.org/MPL/
//
//	Software distributed under the License is distributed on an "AS IS"
//	basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
//	License for the specific language governing rights and limitations
//	under the License.
//
//	The Original Code is "IBPP 0.9" and all its associated documentation.
//
//	The Initial Developer of the Original Code is T.I.P. Group S.A.
//	Portions created by T.I.P. Group S.A. are
//	Copyright (C) 2000 T.I.P Group S.A.
//	All Rights Reserved.
//
//	Contributor(s): ______________________________________.
//
///////////////////////////////////////////////////////////////////////////////
//
//	COMMENTS
//	* Tabulations should be set every four characters when editing this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "ibpp.h"
#include "_internals.h"

#ifdef HAS_HDRSTOP
#pragma hdrstop
#endif

using namespace IBPP;

	/*
	 *	Class Value is a kind of Variant as known in Windows COM. It is a
	 *	'one-size-fits-all' kind of container which can hold the individual
	 *	value of any column type. This class is not fundamental to IBPP. It
	 *	could be implemented in the application programs which need it, but
	 *	there are some advantages to have it offered by IBPP. It is mainly used
	 *	in application which have to handle generic output from a database.
	 *	When the programmer does not know in advance the type of columns
	 *	returned by a SELECT, he/she can use IBPP methods to discover those types,
	 *	but this often leads to runtime conditional code to Get() values
	 *	using an appropriately typed variable. IBPP::Value can simplify the
	 *	writing in such cases.
	 */
	
	class Value
	{
	private:
		enum {vEmpty, vBool, vString, vShort, vInt, vLong, vInt64,
				vFloat, vDouble, vTimestamp, vDate, vTime, vDBKey,
				vBlob, vArray} mType;
		union
		{
			bool mBool;
			std::string* pString;
			short mShort;
			int mInt;
			//long mLong;
			int64_t mInt64;
			float mFloat;
			double mDouble;
			Timestamp* pTimestamp;
			Date* pDate;
			Time* pTime;
			DBKey* pDBKey;
			IBlob* pBlob;
			IArray* pArray;
		} mV;
		
	public:
		virtual void clear();
	
		virtual operator bool() const;
		virtual operator std::string() const;
		virtual operator short() const;
		virtual operator int() const;
		//virtual operator long() const;
		virtual operator int64_t() const;
		virtual operator float() const;
		virtual operator double() const;
		virtual operator Timestamp() const;
		virtual operator Date() const;
		virtual operator Time() const;
		virtual operator DBKey() const;
		virtual operator Blob() const;
		virtual operator Array() const;

		virtual Value& operator=(const bool& rv);
		virtual Value& operator=(const std::string& rv);
		virtual Value& operator=(const short& rv);
		virtual Value& operator=(const int& rv);
		//virtual Value& operator=(const long& rv);
		virtual Value& operator=(const int64_t& rv);
		virtual Value& operator=(const float& rv);
		virtual Value& operator=(const double& rv);
		virtual Value& operator=(const Timestamp& rv);
		virtual Value& operator=(const Date& rv);
		virtual Value& operator=(const Time& rv);
		virtual Value& operator=(const DBKey& rv);
		virtual Value& operator=(const Blob& rv);
		virtual Value& operator=(const Array& rv);
		virtual Value& operator=(const Value& rv);

		Value();
		Value(bool rv);
		Value(const std::string& rv);
		Value(short rv);
		Value(int rv);
		//Value(long rv);
		Value(int64_t rv);
		Value(float rv);
		Value(double rv);
		Value(const Timestamp& rv);
		Value(const Date& rv);
		Value(const Time& rv);
		Value(const DBKey& rv);
		Value(const Blob& rv);
		Value(const Array& rv);
		Value(const Value& rv);

		virtual ~Value();
	};
	
void Value::clear(void)
{
	if (mType == vString)			{ delete mV.mString; mV.mString = 0; }
	else if (mType == vTimestamp)	{ delete mV.mTimestamp; mV.mTimestamp = 0; }
	else if (mType == vDate)		{ delete mV.mDate; mV.mDate = 0; }
	else if (mType == vTime)		{ delete mV.mTime; mV.mTime = 0; }
	else if (mType == vDBKey)		{ delete mV.mDBKey; mV.mDBKey = 0; }
	else if (mType == vBlob)		{ mV.mBlob->Release(mV.mBlob); }
	else if (mType == vArray)		{ mV.mArray->Release(mV.mArray); }
	mType = vEmpty;
}

///////////////////////
// Conversion Operators
///////////////////////

Value::operator bool() const
{
	if (mType != vBool)
		throw LogicExceptionImpl("Value::operator bool()", _("Value is not a bool"));
	return mV.mBool;
}

Value::operator std::string()	const
{
	if (mType != vString)
		throw LogicExceptionImpl("Value::operator std::string()", _("Value is not a std::string"));
	return *mV.mString;
}

Value::operator short() const
{
	if (mType != vShort)
		throw LogicExceptionImpl("Value::operator short()", _("Value is not a short"));
	return mV.mShort;
}

Value::operator int() const
{
	if (mType != vInt)
		throw LogicExceptionImpl("Value::operator int()", _("Value is not an int"));
	return mV.mInt;
}

/*
Value::operator long() const
{
	if (mType != vLong)
		throw LogicExceptionImpl("Value::operator long()", _("Value is not a long"));
	return mV.mLong;
}
*/

Value::operator int64_t() const
{
	if (mType != vInt64)
		throw LogicExceptionImpl("Value::operator int64()", _("Value is not an int64"));
	return mV.mInt64;
}

Value::operator float() const
{
	if (mType != vFloat)
		throw LogicExceptionImpl("Value::operator float()", _("Value is not a float"));
	return mV.mFloat;
}

Value::operator double() const
{
	if (mType != vDouble)
		throw LogicExceptionImpl("Value::operator double()", _("Value is not a double"));
	return mV.mDouble;
}

Value::operator Timestamp() const
{
	if (mType != vTimestamp)
		throw LogicExceptionImpl("Value::operator Timestamp()", _("Value is not a timestamp"));
	return *mV.mTimestamp;
}

Value::operator Date() const
{
	if (mType != vDate)
		throw LogicExceptionImpl("Value::operator Date()", _("Value is not a date"));
	return *mV.mDate;
}

Value::operator Time() const
{
	if (mType != vTime)
		throw LogicExceptionImpl("Value::operator Time()", _("Value is not a time"));
	return *mV.mTime;
}

Value::operator DBKey() const
{
	if (mType != vDBKey)
		throw LogicExceptionImpl("Value::operator DBKey()", _("Value is not a DBKey"));
	return *mV.mDBKey;
}

Value::operator Blob() const
{
	if (mType != vBlob)
		throw LogicExceptionImpl("Value::operator Blob()", _("Value is not a Blob"));
	return Blob(mV.mBlob);
}

Value::operator Array() const
{
	if (mType != vArray)
		throw LogicExceptionImpl("Value::operator Array()", _("Value is not an Array"));
	return Array(mV.mArray);
}

///////////////////////
// Assignment Operators
///////////////////////

Value& Value::operator=(const bool& rv)
{
	clear();
	mType = vBool;
	mV.mBool = rv;
	return *this;
}

Value& Value::operator=(const char* rv)
{
	clear();
	mType = vString;
	mV.mString = new std::string(rv);
	return *this;
}

Value& Value::operator=(const std::string& rv)
{
	clear();
	mType = vString;
	mV.mString = new std::string(rv);
	return *this;
}

Value& Value::operator=(const short& rv)
{
	clear();
	mType = vShort;
	mV.mShort = rv;
	return *this;
}

Value& Value::operator=(const int& rv)
{
	clear();
	mType = vInt;
	mV.mInt = rv;
	return *this;
}

/*
Value& Value::operator=(const long& rv)
{
	clear();
	mType = vLong;
	mV.mLong = rv;
	return *this;
}
*/

Value& Value::operator=(const int64_t& rv)
{
	clear();
	mType = vInt64;
	mV.mInt64 = rv;
	return *this;
}

Value& Value::operator=(const float& rv)
{
	clear();
	mType = vFloat;
	mV.mFloat = rv;
	return *this;
}

Value& Value::operator=(const double& rv)
{
	clear();
	mType = vDouble;
	mV.mDouble = rv;
	return *this;
}

Value& Value::operator=(const Timestamp& rv)
{
	clear();
	mType = vTimestamp;
	mV.mTimestamp = new Timestamp(rv);
	return *this;
}

Value& Value::operator=(const Date& rv)
{
	clear();
	mType = vDate;
	mV.mDate = new Date(rv);
	return *this;
}

Value& Value::operator=(const Time& rv)
{
	clear();
	mType = vTime;
	mV.mTime = new Time(rv);
	return *this;
}

Value& Value::operator=(const DBKey& rv)
{
	clear();
	mType = vDBKey;
	mV.mDBKey = new DBKey(rv);
	return *this;
}

Value& Value::operator=(const Blob& rv)
{
	IBlob* tmp = rv->AddRef();
	clear();
	mType = vBlob;
	mV.mBlob = tmp;
	return *this;
}

Value& Value::operator=(const Array& rv)
{
	IArray* tmp = rv->AddRef();
	clear();
	mType = vArray;
	mV.mArray = tmp;
	return *this;
}

Value& Value::operator=(const Value& rv)
{
	Type tmpType = rv.mType;
	if (tmpType == vString)
	{
		std::string* tmp = new std::string(*rv.mV.mString);
		clear();
		mV.mString = tmp;
	}
	else if (tmpType == vTimestamp)
	{
		Timestamp* tmp = new Timestamp(*rv.mV.mTimestamp);
		clear();
		mV.mTimestamp = tmp;
	}
	else if (tmpType == vDate)
	{
		Date* tmp = new Date(*rv.mV.mDate);
		clear();
		mV.mDate = tmp;
	}
	else if (tmpType == vTime)
	{
		Time* tmp = new Time(*rv.mV.mTime);
		clear();
		mV.mTime = tmp;
	}
	else if (tmpType == vDBKey)
	{
		DBKey* tmp = new DBKey(*rv.mV.mDBKey);
		clear();
		mV.mDBKey = tmp;
	}
	else if (tmpType == vBlob)
	{
		IBlob* tmp = rv.mV.mBlob->AddRef();
		clear();
		mV.mBlob = tmp;
	}
	else if (tmpType == vArray)
	{
		IArray* tmp = rv.mV.mArray->AddRef();
		clear();
		mV.mArray = tmp;
	}
	else mV = rv.mV;

	mType = tmpType;
	return *this;
}

////////////////////
// Copy Constructors
////////////////////

Value::Value(const bool& rv)
{
	mV.mBool = rv; mType = vBool;
}

Value::Value(const char* rv)
{
	mV.mString = new std::string(rv); mType = vString;
}

Value::Value(const std::string& rv)
{
	mV.mString = new std::string(rv); mType = vString;
}

Value::Value(const short& rv)
{
	mV.mShort = rv; mType = vShort;
}

Value::Value(const int& rv)
{
	mV.mInt = rv; mType = vInt;
}

/*
Value::Value(const long& rv)
{
	mV.mLong = rv; mType = vLong;
}
*/

Value::Value(const int64_t& rv)
{
	mV.mInt64 = rv; mType = vInt64;
}

Value::Value(const float& rv)
{
	mV.mFloat = rv; mType = vFloat;
}

Value::Value(const double& rv)
{
	mV.mDouble = rv; mType = vDouble;
}

Value::Value(const Timestamp& rv)
{
	mV.mTimestamp = new Timestamp(rv); mType = vTimestamp;
}

Value::Value(const Date& rv)
{
	mV.mDate = new Date(rv); mType = vDate;
}

Value::Value(const Time& rv)
{
	mV.mTime = new Time(rv); mType = vTime;
}

Value::Value(const DBKey& rv)
{
	mV.mDBKey = new DBKey(rv); mType = vDBKey;
}

Value::Value(const Blob& rv)
{
	mV.mBlob = rv->AddRef(); mType = vBlob;
}

Value::Value(const Array& rv)
{
	mV.mArray = rv->AddRef(); mType = vArray;
}

Value::Value(const Value& rv)
{
	mType = rv.mType;
	if (mType == vString)			{ mV.mString = new std::string(*rv.mV.mString); }
	else if (mType == vTimestamp)	{ mV.mTimestamp = new Timestamp(*rv.mV.mTimestamp); }
	else if (mType == vDate)		{ mV.mDate = new Date(*rv.mV.mDate); }
	else if (mType == vTime)		{ mV.mTime = new Time(*rv.mV.mTime); }
	else if (mType == vDBKey)		{ mV.mDBKey = new DBKey(*rv.mV.mDBKey); }
	else if (mType == vBlob)		{ mV.mBlob = rv.mV.mBlob->AddRef(); }
	else if (mType == vArray)		{ mV.mArray = rv.mV.mArray->AddRef(); }
	else mV = rv.mV;
}

//	Eof
