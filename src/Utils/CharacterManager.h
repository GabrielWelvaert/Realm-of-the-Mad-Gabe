#pragma once

#include <iostream> 
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "../ECS/ECS.h"
#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/PlayerItemsComponent.h"
#include <filesystem>

class CharacterManager {
    private:
        const int lineNumberOfHash = 7; // line where hash resides in character txt file. first line is line 1. 
        const std::string characterFolderPath = (std::filesystem::current_path() / "data/characters").string();
        std::filesystem::directory_iterator dirItr;

        bool FileHasValidLineCount(const std::string& filename);

    public:
        CharacterManager();

        int GetFileCountInCharacterDirectory();

        void KillInvalidCharacterFiles();

        void KillExcessCharacterFiles();

        std::string GetHashCharacterFile(const std::string& filename);

        bool ValidateCharacterFile(const std::string& filename);

        std::string CreateNewCharacterFile(classes className);

        void SaveCharacter(const std::string& activeCharacterID, Entity player);

        std::vector<std::string> GetAllCharacterValuesAtLineNumber(int linenumber);

        std::vector<int> GetLineValuesFromCharacterFile(const std::string& fileName, int linenumber);
};
