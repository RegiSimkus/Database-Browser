#include "SQLiteDatabaseStore.h"
#include "Helpers/Files.h"

namespace SQLiteDatabaseStore
{
	std::vector<CSQLiteDatabase*> Store = {};
}

void SQLiteDatabaseStore::ImportDatabaseFromUser()
{
	char szFileName[MAX_PATH];

	printf("Importing...\n");
	if (!Helpers::File::SelectFilePrompt(szFileName))
	{
		printf("Failed: %s\n", szFileName);
		return;
	}

	printf("found %s\n", szFileName);

	CSQLiteDatabase* db = new CSQLiteDatabase(szFileName);
	printf("db -> %d\n", db->bInvalid ? 1 : 0);
	if (db->bInvalid)
		return;
	printf("stored\n");
	Store.push_back(db);
}

int SQLiteDatabaseStore::CacheDatabaseTablesCallback(void* data, int argc, char** argv, char** pszColumn)
{
	CSQLiteDatabase* database = (CSQLiteDatabase*)data;

	char* name = nullptr;
	char* tbl_name = nullptr;

	for (int i = 0; i < argc; i++) {
		if (!strcmp(pszColumn[i], "tbl_name"))
			tbl_name = argv[i];
		if (!strcmp(pszColumn[i], "name"))
			name = argv[i];
	}

	if ((name == nullptr || tbl_name == nullptr) || (strcmp(name, tbl_name) != 0))
		return 0;

	sqlite_table_t* table = new sqlite_table_t();
	table->name = name;
	table->database = database;

	database->cachedTables.insert({ name, table });

	printf("\n");
	return 0;
}