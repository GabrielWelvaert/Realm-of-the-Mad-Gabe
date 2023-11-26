#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <iostream> 
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <bitset>
#include "../Components/BaseStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Components/ItemComponent.h"
#include "../Components/LootBagComponent.h"
#include "../ECS/ECS.h"
#include "../Systems/VaultSystem.h"
#include "../Utils/Xoshiro256.h"
#include "../Utils/tables.h"
#include <filesystem>

class CharacterManager {
    private:
        Xoshiro256 RNG;
        const int characterLineNumberOfHash = 7; // line where hash resides in character txt file. first line is line 1. 
        const int vaultLineNumberOfHash = 2; // line where hash resides in character txt file. first line is line 1. 
        const std::string characterFolderPath = (std::filesystem::current_path() / "data/characters").string();
        const std::string vaultFolderPath = (std::filesystem::current_path() / "data/vaults").string();
        const std::string nameFolderPath = (std::filesystem::current_path() / "data/name").string();
        std::filesystem::directory_iterator dirItr;

        bool CharacterFileHasValidLineCount(const std::string& filename);
        bool VaultFileHasValidLineCount(const std::string& filename);

    public:
        CharacterManager();

        // methods for character management
        int GetFileCountInCharacterDirectory();
        void KillInvalidCharacterFiles();
        void KillExcessCharacterFiles();
        std::string GetHashCharacterFile(const std::string& filename);
        bool ValidateCharacterFileHash(const std::string& filename);
        std::string CreateNewCharacterFile(classes className);
        void SaveCharacter(const std::string& activeCharacterID, Entity player);
        std::vector<std::string> GetAllCharacterValuesAtLineNumber(int linenumber);
        std::vector<int> GetLineValuesFromCharacterFile(const std::string& fileName, int linenumber);
        void KillCharacter(const std::string& filename);

        // methods for vault management
        int GetFileCountInVaultDirectory();
        void KillInvalidVaultFiles();
        std::string GetHashVaultFile(const std::string& fileName);
        bool ValidateVaultFileHash(const std::string& fileName);
        void CreateNewVaultFile(const std::string& fileName);
        void SaveVaults(std::unique_ptr<Registry>& registry);
        std::vector<int> GetItemsFromVault(int vaultID);

        // methods for name management
        void KillInvalidNameFiles();
        std::string GetName();
        void SaveName(std::string name);
};

#endif