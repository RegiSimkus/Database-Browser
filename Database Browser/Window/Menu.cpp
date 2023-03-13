#include "Menu.h"

namespace Menu
{
    sqlite_table_t* pSelectedTable = nullptr;
    void DrawMenu()
    {
        if (ImGui::Begin("MainAppWindow", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Columns(2);
            {
                static bool bInitColumnSizes = false;
                if (!bInitColumnSizes)
                    ImGui::SetColumnWidth(0, 300.f);
                bInitColumnSizes = true;
            }

            DrawSidebar();

            ImGui::NextColumn();

            ImGui::BeginChild("MainBody", ImGui::GetContentRegionAvail(), true);
            {

                if (pSelectedTable)
                {
                    ImGui::Text("Viewing database: %s", pSelectedTable->name.c_str());
                    ImGui::Separator();

                    ImGui::BeginTable("#SelectedTableData", pSelectedTable->columns.size(), ImGuiTableFlags_Borders);

                    for (auto& colname : pSelectedTable->columns)
                    {
                        ImGui::TableSetupColumn(colname.c_str());
                    }
                    ImGui::TableHeadersRow();

                
                    for (int row = 0; row < pSelectedTable->data.data.size(); row++)
                    {
                        ImGui::TableNextRow();
                        std::map<std::string, std::any>& dat = pSelectedTable->data.data.at(row);
                        for (int col = 0; col < pSelectedTable->columns.size(); col++)
                        {
                            ImGui::TableSetColumnIndex(col);

                            std::string& colName = pSelectedTable->columns.at(col);
                            std::any& value = dat[colName];
                            std::string strVal = value.has_value() ? std::any_cast<std::string&>(value) : "NULL";

                            ImGui::Text(strVal.c_str());
                        }
                    }
                    ImGui::EndTable();

                    // TODO: add SQLite query controls, query text input
                }
                ImGui::EndChild();
            }




            ImGui::End();
        }
    }
}