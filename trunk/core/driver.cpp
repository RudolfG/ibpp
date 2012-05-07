///////////////////////////////////////////////////////////////////////////////
//
//	File    : $Id: $
//	Subject : IBPP, Driver class implementation
//
///////////////////////////////////////////////////////////////////////////////
//
//	(C) Copyright 2000-2007 T.I.P. Group S.A. and the IBPP Team (www.ibpp.org)
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

#include <iostream>

#ifdef IBPP_WINDOWS
// New (optional) Registry Keys introduced by Firebird Server 1.5
#define REG_KEY_ROOT_INSTANCES	"SOFTWARE\\Firebird Project\\Firebird Server\\Instances"
#define FB_DEFAULT_INSTANCE	  	"DefaultInstance"
#endif

using namespace ibpp_internals;

//	(((((((( OBJECT INTERFACE IMPLEMENTATION ))))))))

void DriverImpl::Load(const std::string& optPaths)
{
	// Let's load the driver library, if it is not already loaded.

	if (! mLoaded)
	{
#ifdef IBPP_WINDOWS

		// Let's load the FBCLIENT.DLL or GDS32.DLL, we will never release it.
		// Windows will do that for us when the executable will terminate.

		char fbdll[MAX_PATH];
		HKEY hkey_instances;
		std::string AppPath;

		// Try to load FBCLIENT.DLL from each of the additional optional paths
		// that may have been specified through ClientLibSearchPaths().
		// We also want to actually update the environment PATH so that it references
		// the specific path from where we attempt the load. This is useful because
		// it directs the system to attempt finding dependencies (like the C/C++
		// runtime libraries) from the same location where FBCLIENT is found.

		mHandle = 0;
		std::string failed_paths;
		std::string SysPath(getenv("PATH"));
		std::string::size_type pos = 0;
		while (pos < optPaths.size())
		{
			std::string::size_type newpos = optPaths.find(';', pos);

			std::string path;
			if (newpos == std::string::npos) path = optPaths.substr(pos);
			else path = optPaths.substr(pos, newpos-pos);

			if (path.size() >= 1)
			{
				if (path[path.size()-1] != '\\') path += '\\';

				AppPath.assign("PATH=");
				AppPath.append(path).append(";").append(SysPath);
				putenv(AppPath.c_str());

				std::string dll(path);
				dll.append("fbclient.dll");
				mHandle = LoadLibrary(dll.c_str());
				if (mHandle != 0) break;
				failed_paths.append(dll).append("\n");

				dll.assign(path).append("fbembed.dll");
				mHandle = LoadLibrary(dll.c_str());
				if (mHandle != 0) break;
				failed_paths.append(dll).append("\n");
				if (newpos == std::string::npos) break;
			}
			pos = newpos + 1;
		}

		if (mHandle == 0)
		{
			// Try to load FBCLIENT.DLL from the current application location.  This
			// is a usefull step for applications using the embedded version of FB
			// or a local copy (for whatever reasons) of the dll.

			if (! AppPath.empty())
			{
				// Restores the original system path
				AppPath.assign("PATH=");
				AppPath.append(SysPath);
				putenv(AppPath.c_str());
			}

			int len = GetModuleFileName(NULL, fbdll, sizeof(fbdll));
			if (len != 0)
			{
				// Get to the last '\' (this one precedes the filename part).
				// There is always one after a success call to GetModuleFileName().
				char* p = fbdll + len;
				do {--p;} while (*p != '\\');
				*p = '\0';
				lstrcat(fbdll, "\\fbembed.dll");// Local copy could be named fbembed.dll
				mHandle = LoadLibrary(fbdll);
				if (mHandle == 0)
				{
					failed_paths.append(fbdll).append("\n");
					*p = '\0';
					lstrcat(fbdll, "\\fbclient.dll");	// Or possibly renamed fbclient.dll
					mHandle = LoadLibrary(fbdll);
				}
			}
		}

		if (mHandle == 0)
		{
			// Try to locate FBCLIENT.DLL through the optional FB registry key.

			failed_paths.append(fbdll).append("\n");
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_ROOT_INSTANCES, 0,
				KEY_READ, &hkey_instances) == ERROR_SUCCESS)
			{
				DWORD keytype;
				DWORD buflen = sizeof(fbdll);
				if (RegQueryValueEx(hkey_instances, FB_DEFAULT_INSTANCE, 0,
						&keytype, reinterpret_cast<UCHAR*>(fbdll),
							&buflen) == ERROR_SUCCESS && keytype == REG_SZ)
				{
					lstrcat(fbdll, "bin\\fbclient.dll");
					mHandle = LoadLibrary(fbdll);
					if (mHandle == 0) failed_paths.append(fbdll).append("\n");
				}
				RegCloseKey(hkey_instances);
			}
		}

		if (mHandle == 0)
		{
			// Let's try from the PATH and System directories
			mHandle = LoadLibrary("fbclient.dll");
			if (mHandle == 0)
			{
				// Not found. Last try : attemps loading gds32.dll from PATH and
				// System directories
				failed_paths.append("fbclient.dll").append("\n");
				mHandle = LoadLibrary("gds32.dll");
				if (mHandle == 0)
				{
					failed_paths.append("gds32.dll").append("\n");
					failed_paths.insert(0, "\n").insert(0, _("Can't find/load FBCLIENT.DLL/FBEMBED.DLL/GDS32.DLL through these paths:"));
					throw LogicExceptionImpl("Driver::Load()",
						_("Can't find or load FBCLIENT.DLL or GDS32.DLL"));
				}
			}
		}
#endif

		mGDSVersion = 60;

		// Get the entry points that we need

#ifdef IBPP_WINDOWS
#define IB_ENTRYPOINT(X) \
			if ((m_##X = (proto_##X*)GetProcAddress(mHandle, "isc_"#X)) == 0) \
			throw LogicExceptionImpl("Driver::Load()", _("Entry-point isc_"#X" not found"))
#define FB_ENTRYPOINT(X) \
			if ((m_##X = (proto_##X*)GetProcAddress(mHandle, "fb_"#X)) == 0) \
				throw LogicExceptionImpl("Driver::Load()", _("Entry-point fb_"#X" not found"))
#endif
#ifdef IBPP_UNIX
/* TODO : perform a late-bind on unix */
#define IB_ENTRYPOINT(X) m_##X = (proto_##X*)isc_##X
#define FB_ENTRYPOINT(X) m_##X = (proto_##X*)fb_##X
#endif

		IB_ENTRYPOINT(create_database);
		IB_ENTRYPOINT(attach_database);
		IB_ENTRYPOINT(detach_database);
		IB_ENTRYPOINT(drop_database);
		IB_ENTRYPOINT(database_info);
		IB_ENTRYPOINT(open_blob2);
		IB_ENTRYPOINT(create_blob2);
		IB_ENTRYPOINT(close_blob);
		IB_ENTRYPOINT(cancel_blob);
		IB_ENTRYPOINT(get_segment);
		IB_ENTRYPOINT(put_segment);
		IB_ENTRYPOINT(blob_info);
		IB_ENTRYPOINT(array_lookup_bounds);
		IB_ENTRYPOINT(array_get_slice);
		IB_ENTRYPOINT(array_put_slice);
		IB_ENTRYPOINT(vax_integer);
		IB_ENTRYPOINT(sqlcode);
		IB_ENTRYPOINT(sql_interprete);
		IB_ENTRYPOINT(interprete);
		IB_ENTRYPOINT(que_events);
		IB_ENTRYPOINT(cancel_events);
		IB_ENTRYPOINT(start_multiple);
		IB_ENTRYPOINT(commit_transaction);
		IB_ENTRYPOINT(commit_retaining);
		IB_ENTRYPOINT(rollback_transaction);
		IB_ENTRYPOINT(rollback_retaining);
		IB_ENTRYPOINT(dsql_execute_immediate);
		IB_ENTRYPOINT(dsql_allocate_statement);
		IB_ENTRYPOINT(dsql_describe);
		IB_ENTRYPOINT(dsql_describe_bind);
		IB_ENTRYPOINT(dsql_prepare);
		IB_ENTRYPOINT(dsql_execute);
		IB_ENTRYPOINT(dsql_execute2);
		IB_ENTRYPOINT(dsql_fetch);
		IB_ENTRYPOINT(dsql_free_statement);
		IB_ENTRYPOINT(dsql_set_cursor_name);
		IB_ENTRYPOINT(dsql_sql_info);

		IB_ENTRYPOINT(service_attach);
		IB_ENTRYPOINT(service_detach);
		IB_ENTRYPOINT(service_start);
		IB_ENTRYPOINT(service_query);

		FB_ENTRYPOINT(shutdown);

		mLoaded = true;
	}
}

void DriverImpl::Unload()
{
	if (mLoaded)
	{
		std::set<IBPP::IInterface*>::iterator it = mInterfaces.begin();
		while (it != mInterfaces.end())
		{
			DatabaseImpl* db = dynamic_cast<DatabaseImpl*>(*it);
			if (db != 0 && db->Connected())
				db->Disconnect();
			else
			{
				ServiceImpl* sv = dynamic_cast<ServiceImpl*>(*it);
				if (sv != 0 && sv->Connected())
					sv->Disconnect();
			}
			it++;
		}

#ifdef IBPP_WINDOWS
		if (mHandle != 0)
		{
			FreeLibrary(mHandle);
			mHandle = 0;
		}
#endif
		mLoaded = false;
	}
}

bool DriverImpl::EmbeddedShutdown(unsigned int timeout, const int reason)
{
	// The reason code passed by an application should be positive or null.
	// We turn a negative value to positive.
	bool res = false;
	try
	{
		res = (*m_shutdown)(timeout, reason < 0 ? -reason : reason) == 0;
	}
	catch(...) { /**/ }

	return res;
}


void DriverImpl::GetVersion(std::string &version)
{
	version.erase();
}

IBPP::Service DriverImpl::ServiceFactory(const std::string& ServerName,
				const std::string& UserName, const std::string& UserPassword)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IService* pi = new ServiceImpl(this, ServerName, UserName, UserPassword);
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Database DriverImpl::DatabaseFactory(const std::string& ServerName,
		const std::string& DatabaseName, const std::string& UserName,
		const std::string& UserPassword, const std::string& RoleName,
		const std::string& CharSet, const std::string& CreateParams)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IDatabase* pi = new DatabaseImpl(this, ServerName, DatabaseName, UserName,
								UserPassword, RoleName, CharSet, CreateParams);
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Transaction DriverImpl::TransactionFactory(IBPP::Database db, IBPP::TAM am,
					IBPP::TIL il, IBPP::TLR lr, IBPP::TFF flags)
{
	Load();			// Triggers the initialization, if needed
	IBPP::ITransaction* pi = new TransactionImpl(this, dynamic_cast<DatabaseImpl*>(db.intf()),
									am, il, lr, flags);
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Statement DriverImpl::StatementFactory(IBPP::Database db, IBPP::Transaction tr,
		const std::string& sql)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IStatement* pi = new StatementImpl(this, dynamic_cast<DatabaseImpl*>(db.intf()),
									dynamic_cast<TransactionImpl*>(tr.intf()),
										sql);
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Blob DriverImpl::BlobFactory(IBPP::Database db, IBPP::Transaction tr)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IBlob* pi = new BlobImpl(this, dynamic_cast<DatabaseImpl*>(db.intf()),
							dynamic_cast<TransactionImpl*>(tr.intf()));
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Array DriverImpl::ArrayFactory(IBPP::Database db, IBPP::Transaction tr)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IArray* pi = new ArrayImpl(this, dynamic_cast<DatabaseImpl*>(db.intf()),
							dynamic_cast<TransactionImpl*>(tr.intf()));
	mInterfaces.insert(pi);
	return pi;
}

IBPP::Events DriverImpl::EventsFactory(IBPP::Database db)
{
	Load();			// Triggers the initialization, if needed
	IBPP::IEvents* pi = new EventsImpl(this, dynamic_cast<DatabaseImpl*>(db.intf()));
	mInterfaces.insert(pi);
	return pi;
}

IBPP::IDriver* DriverImpl::AddRef()
{
	ASSERTION(mRefCount >= 0);
	++mRefCount;
	return this;
}

void DriverImpl::Release()
{
	// Release cannot throw, except in DEBUG builds on assertion
	ASSERTION(mRefCount >= 0);
	--mRefCount;
	try { if (mRefCount <= 0) delete this; }
		catch (...) { }
}

namespace IBPP
{
	Driver DriverFactory()
	{
		return new DriverImpl();
	}
}

//	(((((((( OBJECT INTERNAL METHODS ))))))))

void DriverImpl::Detach(IBPP::IInterface* pi)
{
	mInterfaces.erase(pi);
}

DriverImpl::DriverImpl()
: mRefCount(0)
{
	mLoaded = false;
	mGDSVersion = 0;
#ifdef IBPP_WINDOWS
	mHandle = 0;
#endif
}

DriverImpl::~DriverImpl()
{
	Unload();
}

//
//	EOF
//
