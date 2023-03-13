#pragma once

#include <Windows.h>

#include <vector>
#include <string>
#include <sqlite/sqlite3.h>
#include <map>
#include <any>

class CSQLiteDatabase;
struct sqlite_table_t;

namespace SQLiteDatabaseStore
{
	extern std::vector<CSQLiteDatabase*> Store;

	void ImportDatabaseFromUser();
	int CacheDatabaseTablesCallback(void*, int, char**, char**);
}

class SQLiteTableData
{
public:
	bool GetBool(int row, std::string column)
	{
		std::any& value = data[row][column];
		return std::any_cast<bool>(value);
	}

	std::string GetString(int row, std::string column) 
	{
		std::any& value = data[row][column];
		return std::any_cast<std::string>(value);
	}

	std::vector<std::map<std::string, std::any>> data;
};


class CSQLiteDatabase
{
public:
	CSQLiteDatabase(char* szDir)
	{
		Dir = szDir;
		FileName = Dir.substr(Dir.find_last_of("\\") + 1);

		printf("initializing db: %s\n", szDir);
		if (sqlite3_open(szDir, &this->pDatabase) != SQLITE_OK)
		{
			const char* error = sqlite3_errmsg(this->pDatabase);
			MessageBoxA(NULL, error, "SQLite DB Error", MB_OK);
			bInvalid = true;
			return;
		}
		else
			bInvalid = false;
		
		char* szError;
		sqlite3_exec(this->pDatabase, "SELECT * FROM sqlite_master;", SQLiteDatabaseStore::CacheDatabaseTablesCallback, (void*)this, &szError);
		sqlite3_close(this->pDatabase);
	}

	~CSQLiteDatabase()
	{
		if (pDatabase)
		{
			bInvalid = true;
			sqlite3_close(pDatabase);
			pDatabase = nullptr;
		}
	}

	sqlite3* Open()
	{
		if (sqlite3_open(Dir.c_str(), &pDatabase) != SQLITE_OK)
			return nullptr;

		return pDatabase;
	}

	void Close()
	{
		if (!pDatabase)
			return;

		sqlite3_close(pDatabase);
	}

	std::map<std::string, sqlite_table_t*> cachedTables;
	sqlite3* pDatabase;
	std::string Dir;
	std::string FileName;
	bool bInvalid = true;
};


struct sqlite_table_t
{
protected:
	static int GetDataCallback(void* param, int argc, char** argv, char** pszColumn)
	{
		sqlite_table_t* table = (sqlite_table_t*)param;

		std::map<std::string, std::any> row = std::map<std::string, std::any>();
		for (int i = 0; i < argc; i++)
		{
			std::string colname = pszColumn[i];
			printf("caching row[%s] = %s\n", colname.c_str(), argv[i]);
			row[colname] = std::string(argv[i] ? argv[i] : "NULL");
		}
		table->data.data.push_back(row);
		return 0;
	}

	static int GetHeadersCallback(void* param, int argc, char** argv, char** pszColumn)
	{
		sqlite_table_t* table = (sqlite_table_t*)param;
		for (int i = 0; i < argc; i++)
		{
			if (strcmp(pszColumn[i], "name") == 0)
			{
				table->columns.push_back(argv[i] ? argv[i] : "NULL!?!?");
			}
		}
		return 0;
	}

public:
	std::string name;
	SQLiteTableData data;
	std::vector<std::string> columns;
	CSQLiteDatabase* database;

	void Refresh()
	{
		std::string query = "PRAGMA table_info(`" + name + "`)";
		const char* szTail;
		sqlite3_stmt* pStatement;

		sqlite3* db = this->database->Open();

		char* error;
		int rc;

		data.data.clear();
		columns.clear();
		if ((rc = sqlite3_exec(db, query.c_str(), GetHeadersCallback, (void*)this, &error)) != SQLITE_OK)
		{
			printf("error!\n");
		}

		query = "SELECT * FROM `" + name + "`";
		if ((rc = sqlite3_exec(db, query.c_str(), GetDataCallback, (void*)this, &error)) != SQLITE_OK)
		{
			printf("error!\n");
		}

		this->database->Close();
		// printf("tail -> %s\n", szTail);
	}
};