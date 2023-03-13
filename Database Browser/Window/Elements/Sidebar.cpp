#include "../Menu.h"

void Menu::DrawSidebar()
{
    ImGui::BeginChild("DBSideBarLeft", ImVec2(), true);
    {

        if (ImGui::CollapsingHeader("SQLite Databases"))
        {
            ImGui::BeginChild("##SQLITE_DATABASES_BODY");
            if (ImGui::Button("Import Database"))
            {
                SQLiteDatabaseStore::ImportDatabaseFromUser();
            }
            ImGui::Separator();

            for (auto& db : SQLiteDatabaseStore::Store)
            {
                if (ImGui::CollapsingHeader(db->FileName.c_str()))
                {
                    for (auto& table : db->cachedTables)
                    {
                        if (ImGui::Selectable(table.first.c_str(), pSelectedTable == table.second))
                        {
                            printf("Selected %s\n", table.first.c_str());
                            pSelectedTable = table.second;
                            pSelectedTable->Refresh();
                        }
                    }
                }
            }
            ImGui::EndChild();
        }

        ImGui::EndChild();
    }
}