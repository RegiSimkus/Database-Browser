#pragma once

#include <Windows.h>

namespace Helpers::File
{
	inline bool SelectFilePrompt(char* pszFileName)
	{
		if (!pszFileName)
			return false;

		pszFileName[0] = '\0';

		OPENFILENAMEA file;
		ZeroMemory(&file, sizeof(OPENFILENAMEA));
		file.hwndOwner = NULL;
		file.lStructSize = sizeof(OPENFILENAMEA);
		file.hInstance = GetModuleHandle(NULL);
		file.nMaxFile = MAX_PATH;
		file.lpstrFile = pszFileName;
		// file.lpstrFilter = "All";
		file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
		file.nFilterIndex = -1;

		return GetOpenFileNameA(&file);
	}
}