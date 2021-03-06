///////////////////////////////////////////////////////////////////////////////
//
//	File    : $Id$
//	Subject : IBPP, Database class implementation
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

#include <algorithm>

using namespace ibpp_internals;

//	(((((((( OBJECT INTERFACE IMPLEMENTATION ))))))))

void DatabaseImpl::Create(int dialect)
{
	if (mHandle != 0)
		throw LogicExceptionImpl("Database::Create", _("Database is already connected."));
	if (mDatabaseName.empty())
		throw LogicExceptionImpl("Database::Create", _("Unspecified database name."));
	if (mUserName.empty())
		throw LogicExceptionImpl("Database::Create", _("Unspecified user name."));
	if (dialect != 1 && dialect != 3)
		throw LogicExceptionImpl("Database::Create", _("Only dialects 1 and 3 are supported."));

	// Build the SQL Create Statement
	std::string create;
	create.assign("CREATE DATABASE \"");
	if (! mServerName.empty()) create.append(mServerName).append(":");
	create.append(escape(mDatabaseName, '"')).append("\" ");

	create.append("USER '").append(mUserName).append("' ");
	if (! mUserPassword.empty())
		create.append("PASSWORD '").append(mUserPassword).append("' ");

	if (! mCreateParams.empty()) create.append(mCreateParams);

	// Call ExecuteImmediate to create the database
	isc_tr_handle tr_handle = 0;
	IBS status(mDriver);
    (mDriver->m_dsql_execute_immediate)(status.Self(), &mHandle, &tr_handle,
    	0, const_cast<char*>(create.c_str()), short(dialect), NULL);
    if (status.Errors())
		throw SQLExceptionImpl(status, "Database::Create", _("isc_dsql_execute_immediate failed"));

	Disconnect();
}

void DatabaseImpl::Connect()
{
	if (mHandle != 0) return;	// Already connected

	if (mDatabaseName.empty())
		throw LogicExceptionImpl("Database::Connect", _("Unspecified database name."));
	if (mUserName.empty())
		throw LogicExceptionImpl("Database::Connect", _("Unspecified user name."));

    // Build a DPB based on the properties
	DPB dpb(mDriver);
    dpb.Insert(isc_dpb_user_name, mUserName.c_str());
    dpb.Insert(isc_dpb_password, mUserPassword.c_str());
    if (! mRoleName.empty()) dpb.Insert(isc_dpb_sql_role_name, mRoleName.c_str());
    if (! mCharSet.empty()) dpb.Insert(isc_dpb_lc_ctype, mCharSet.c_str());

	std::string conn;
	if (! mServerName.empty())
		conn.assign(mServerName).append(":");
	conn.append(mDatabaseName);

	IBS status(mDriver);
	(void)(*mDriver->m_attach_database)(status.Self(), (short)conn.size(),
		const_cast<char*>(conn.c_str()), &mHandle, dpb.Size(), dpb.Self());
    if (status.Errors())
    {
        mHandle = 0;     // Should be, but better be sure...
		throw SQLExceptionImpl(status, "Database::Connect", _("isc_attach_database failed"));
    }

	// Now, get ODS version information and dialect.
	// If ODS major is lower of equal to 9, we reject the connection.
	// If ODS major is 10 or higher, this is at least an InterBase 6.x Server
	// OR FireBird 1.x Server.

	char items[] = {(char)isc_info_ods_version,
					(char)isc_info_db_SQL_dialect,
					(char)isc_info_end};
	RB result(mDriver, 100);

	status.Reset();
	(mDriver->m_database_info)(status.Self(), &mHandle, sizeof(items), items,
		result.Size(), result.Self());
	if (status.Errors())
	{
		status.Reset();
	    (mDriver->m_detach_database)(status.Self(), &mHandle);
        mHandle = 0;     // Should be, but better be sure...
		throw SQLExceptionImpl(status, "Database::Connect", _("isc_database_info failed"));
	}

	int ODS = result.GetValue(isc_info_ods_version);
	if (ODS <= 9)
	{
		status.Reset();
	    (mDriver->m_detach_database)(status.Self(), &mHandle);
        mHandle = 0;     // Should be, but better be sure...
		throw LogicExceptionImpl("Database::Connect",
			_("Unsupported Server : wrong ODS version (%d), at least '10' required."), ODS);
	}

	mDialect = result.GetValue(isc_info_db_SQL_dialect);
	if (mDialect != 1 && mDialect != 3)
	{
		status.Reset();
	    (mDriver->m_detach_database)(status.Self(), &mHandle);
        mHandle = 0;     // Should be, but better be sure...
		throw LogicExceptionImpl("Database::Connect", _("Dialect 1 or 3 required"));
	}

	// Now, verify the GDS32.DLL we are using is compatible with the server
	if (ODS >= 10 && mDriver->mGDSVersion < 60)
	{
		status.Reset();
	    (mDriver->m_detach_database)(status.Self(), &mHandle);
        mHandle = 0;     // Should be, but better be sure...
		throw LogicExceptionImpl("Database::Connect", _("GDS32.DLL version 5 against IBSERVER 6"));
	}
}

void DatabaseImpl::Inactivate()
{
	if (mHandle == 0) return;	// Not connected anyway

    // Rollback any started transaction...
	for (unsigned i = 0; i < mTransactions.size(); i++)
	{
		if (mTransactions[i]->Started())
			mTransactions[i]->Rollback();
	}

	// Cancel all pending event traps
	for (unsigned i = 0; i < mEvents.size(); i++)
		mEvents[i]->Clear();

	// Let's detach from all Blobs
	while (mBlobs.size() > 0)
		mBlobs.back()->DetachDatabaseImpl();

	// Let's detach from all Arrays
	while (mArrays.size() > 0)
		mArrays.back()->DetachDatabaseImpl();

	// Let's detach from all Statements
	while (mStatements.size() > 0)
		mStatements.back()->DetachDatabaseImpl();

	// Let's detach from all Transactions
	while (mTransactions.size() > 0)
		mTransactions.back()->DetachDatabaseImpl(this);

	// Let's detach from all Events
	while (mEvents.size() > 0)
		mEvents.back()->DetachDatabaseImpl();
}

void DatabaseImpl::Disconnect()
{
	if (mHandle == 0) return;	// Not connected anyway

	// Put the connection to rest
	Inactivate();

	// Detach from the server
	IBS status(mDriver);
    (mDriver->m_detach_database)(status.Self(), &mHandle);

    // Should we throw, set mHandle to 0 first, because Disconnect() may
	// be called from Database destructor (keeps the object coherent).
	mHandle = 0;
    if (status.Errors())
		throw SQLExceptionImpl(status, "Database::Disconnect", _("isc_detach_database failed"));
}

void DatabaseImpl::Drop()
{
	if (mHandle == 0)
		throw LogicExceptionImpl("Database::Drop", _("Database must be connected."));

	// Put the connection to a rest
	Inactivate();

	IBS status(mDriver);
	(mDriver->m_drop_database)(status.Self(), &mHandle);
    if (status.Errors())
    	throw SQLExceptionImpl(status, "Database::Drop", _("isc_drop_database failed"));

    mHandle = 0;
}

void DatabaseImpl::Info(int* ODSMajor, int* ODSMinor,
	int* PageSize, int* Pages, int* Buffers, int* Sweep,
	bool* Sync, bool* Reserve, bool* ReadOnly)
{
	if (mHandle == 0)
		throw LogicExceptionImpl("Database::Info", _("Database is not connected."));

	char items[] = {(char)isc_info_ods_version,
					(char)isc_info_ods_minor_version,
					(char)isc_info_page_size,
					(char)isc_info_allocation,
					(char)isc_info_num_buffers,
					(char)isc_info_sweep_interval,
					(char)isc_info_forced_writes,
					(char)isc_info_no_reserve,
					(char)isc_info_db_read_only,
					(char)isc_info_end};
    IBS status(mDriver);
	RB result(mDriver, 256);

	status.Reset();
	(mDriver->m_database_info)(status.Self(), &mHandle, sizeof(items), items,
		result.Size(), result.Self());
	if (status.Errors())
		throw SQLExceptionImpl(status, "Database::Info", _("isc_database_info failed"));

	if (ODSMajor != 0) *ODSMajor = result.GetValue((char)isc_info_ods_version);
	if (ODSMinor != 0) *ODSMinor = result.GetValue((char)isc_info_ods_minor_version);
	if (PageSize != 0) *PageSize = result.GetValue((char)isc_info_page_size);
	if (Pages != 0) *Pages = result.GetValue((char)isc_info_allocation);
	if (Buffers != 0) *Buffers = result.GetValue((char)isc_info_num_buffers);
	if (Sweep != 0) *Sweep = result.GetValue((char)isc_info_sweep_interval);
	if (Sync != 0)
		*Sync = result.GetValue((char)isc_info_forced_writes) == 1 ? true : false;
	if (Reserve != 0)
		*Reserve = result.GetValue((char)isc_info_no_reserve) == 1 ? false : true;
	if (ReadOnly != 0)
		*ReadOnly = result.GetValue((char)isc_info_db_read_only) == 1 ? true : false;
}

void DatabaseImpl::Statistics(int* Fetches, int* Marks, int* Reads, int* Writes)
{
	if (mHandle == 0)
		throw LogicExceptionImpl("Database::Statistics", _("Database is not connected."));

	char items[] = {(char)isc_info_fetches,
					(char)isc_info_marks,
					(char)isc_info_reads,
					(char)isc_info_writes,
					(char)isc_info_end};
    IBS status(mDriver);
	RB result(mDriver, 128);

	status.Reset();
	(mDriver->m_database_info)(status.Self(), &mHandle, sizeof(items), items,
		result.Size(), result.Self());
	if (status.Errors())
		throw SQLExceptionImpl(status, "Database::Statistics", _("isc_database_info failed"));

	if (Fetches != 0) *Fetches = result.GetValue((char)isc_info_fetches);
	if (Marks != 0) *Marks = result.GetValue((char)isc_info_marks);
	if (Reads != 0) *Reads = result.GetValue((char)isc_info_reads);
	if (Writes != 0) *Writes = result.GetValue((char)isc_info_writes);
}

void DatabaseImpl::Counts(int* Insert, int* Update, int* Delete, 
	int* ReadIdx, int* ReadSeq)
{
	if (mHandle == 0)
		throw LogicExceptionImpl("Database::Counts", _("Database is not connected."));

	char items[] = {(char)isc_info_insert_count,
					(char)isc_info_update_count,
					(char)isc_info_delete_count,
					(char)isc_info_read_idx_count,
					(char)isc_info_read_seq_count,
					(char)isc_info_end};
    IBS status(mDriver);
	RB result(mDriver, 1024);

	status.Reset();
	(mDriver->m_database_info)(status.Self(), &mHandle, sizeof(items), items,
		result.Size(), result.Self());
	if (status.Errors())
		throw SQLExceptionImpl(status, "Database::Counts", _("isc_database_info failed"));

	if (Insert != 0) *Insert = result.GetCountValue((char)isc_info_insert_count);
	if (Update != 0) *Update = result.GetCountValue((char)isc_info_update_count);
	if (Delete != 0) *Delete = result.GetCountValue((char)isc_info_delete_count);
	if (ReadIdx != 0) *ReadIdx = result.GetCountValue((char)isc_info_read_idx_count);
	if (ReadSeq != 0) *ReadSeq = result.GetCountValue((char)isc_info_read_seq_count);
}

void DatabaseImpl::Users(std::vector<std::string>& users)
{
	if (mHandle == 0)
		throw LogicExceptionImpl("Database::Users", _("Database is not connected."));

	char items[] = {(char)isc_info_user_names,
					(char)isc_info_end};
    IBS status(mDriver);
	RB result(mDriver, 8000);

	status.Reset();
	(mDriver->m_database_info)(status.Self(), &mHandle, sizeof(items), items,
		result.Size(), result.Self());
	if (status.Errors())
	{
		status.Reset();
		throw SQLExceptionImpl(status, "Database::Users", _("isc_database_info failed"));
	}

	users.clear();
	char* p = result.Self();
	while (*p == (char)isc_info_user_names)
	{
		p += 3;		// Get to the length byte (there are two undocumented bytes which we skip)
		int len = (int)(*p);
		++p;		// Get to the first char of username
    	if (len != 0) users.push_back(std::string().append(p, len));
   		p += len;	// Skip username
    }
	return;
}

IBPP::IDatabase* DatabaseImpl::AddRef()
{
	ASSERTION(mRefCount >= 0);
	++mRefCount;
	return this;
}

void DatabaseImpl::Release()
{
	// Release cannot throw, except in DEBUG builds on assertion
	ASSERTION(mRefCount >= 0);
	--mRefCount;
	try
	{
		if (mRefCount <= 0)
		{
			mDriver->Detach(this);
			delete this;
		}
	}
	catch (...) { }
}

//	(((((((( OBJECT INTERNAL METHODS ))))))))

void DatabaseImpl::AttachTransactionImpl(TransactionImpl* tr)
{
	if (tr == 0)
		throw LogicExceptionImpl("Database::AttachTransaction",
					_("Transaction object is null."));

	mTransactions.push_back(tr);
}

void DatabaseImpl::DetachTransactionImpl(TransactionImpl* tr)
{
	if (tr == 0)
		throw LogicExceptionImpl("Database::DetachTransaction",
				_("ITransaction object is null."));

	mTransactions.erase(std::find(mTransactions.begin(), mTransactions.end(), tr));
}

void DatabaseImpl::AttachStatementImpl(StatementImpl* st)
{
	if (st == 0)
		throw LogicExceptionImpl("Database::AttachStatement",
					_("Can't attach a null Statement object."));

	mStatements.push_back(st);
}

void DatabaseImpl::DetachStatementImpl(StatementImpl* st)
{
	if (st == 0)
		throw LogicExceptionImpl("Database::DetachStatement",
				_("Can't detach a null Statement object."));

	mStatements.erase(std::find(mStatements.begin(), mStatements.end(), st));
}

void DatabaseImpl::AttachBlobImpl(BlobImpl* bb)
{
	if (bb == 0)
		throw LogicExceptionImpl("Database::AttachBlob",
					_("Can't attach a null Blob object."));

	mBlobs.push_back(bb);
}

void DatabaseImpl::DetachBlobImpl(BlobImpl* bb)
{
	if (bb == 0)
		throw LogicExceptionImpl("Database::DetachBlob",
				_("Can't detach a null Blob object."));

	mBlobs.erase(std::find(mBlobs.begin(), mBlobs.end(), bb));
}

void DatabaseImpl::AttachArrayImpl(ArrayImpl* ar)
{
	if (ar == 0)
		throw LogicExceptionImpl("Database::AttachArray",
					_("Can't attach a null Array object."));

	mArrays.push_back(ar);
}

void DatabaseImpl::DetachArrayImpl(ArrayImpl* ar)
{
	if (ar == 0)
		throw LogicExceptionImpl("Database::DetachArray",
				_("Can't detach a null Array object."));

	mArrays.erase(std::find(mArrays.begin(), mArrays.end(), ar));
}

void DatabaseImpl::AttachEventsImpl(EventsImpl* ev)
{
	if (ev == 0)
		throw LogicExceptionImpl("Database::AttachEventsImpl",
					_("Can't attach a null Events object."));

	mEvents.push_back(ev);
}

void DatabaseImpl::DetachEventsImpl(EventsImpl* ev)
{
	if (ev == 0)
		throw LogicExceptionImpl("Database::DetachEventsImpl",
				_("Can't detach a null Events object."));

	mEvents.erase(std::find(mEvents.begin(), mEvents.end(), ev));
}

DatabaseImpl::DatabaseImpl(DriverImpl* drv, const std::string& serverName, const std::string& databaseName,
						   const std::string& userName, const std::string& userPassword,
						   const std::string& roleName, const std::string& charSet,
						   const std::string& createParams) :

	mRefCount(0), mDriver(drv), mHandle(0),
	mServerName(serverName), mDatabaseName(databaseName),
	mUserName(userName), mUserPassword(userPassword), mRoleName(roleName),
	mCharSet(charSet), mCreateParams(createParams),
	mDialect(3)
{
}

DatabaseImpl::~DatabaseImpl()
{
	try
	{
		if (DatabaseImpl::Connected())
			DatabaseImpl::Disconnect();
	}
	catch(...) { }
}

std::string ibpp_internals::escape(const std::string& s, char e)
{
	std::string::const_iterator it;
	std::string esc;
	for (it = s.begin(); it != s.end(); it++)
	{
		if (*it == e) esc.push_back('\\');
		esc.push_back(*it);
	}
	return esc;
}

//
//	EOF
//
