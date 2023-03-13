#pragma once
#include "ImGui.h"
#include "../App/SQLiteDatabaseStore.h"

namespace Menu
{
	extern sqlite_table_t* pSelectedTable;

	void DrawSidebar();
	void DrawMenu();
}