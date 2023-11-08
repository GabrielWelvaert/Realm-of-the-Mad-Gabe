#include "CharacterManager.h"

/*
The CharacterManager is responsible for managing files associated with saved characters
It saves, creates, processes, and validates character files, including vault files
*/

CharacterManager::CharacterManager(){
    std::string dataFolderPath = (std::filesystem::current_path() / "data").string();
    if(!std::filesystem::exists(dataFolderPath)){
        std::filesystem::create_directories(dataFolderPath);
    }
    if(!std::filesystem::exists(vaultFolderPath)){
        std::filesystem::create_directories(vaultFolderPath);
    }
    if(!std::filesystem::exists(characterFolderPath)){
        std::filesystem::create_directories(characterFolderPath);
    }
    if(!std::filesystem::exists(nameFolderPath)){
        std::filesystem::create_directories(nameFolderPath);
    }
}

bool CharacterManager::CharacterFileHasValidLineCount(const std::string& filename){
    std::string filePath = (std::filesystem::path(characterFolderPath) / filename).replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    int lineCount = 0;
    while(std::getline(file, line)){
        lineCount++;
    }
    return (lineCount == characterLineNumberOfHash);
}

bool CharacterManager::VaultFileHasValidLineCount(const std::string& filename){
    std::string filePath = (std::filesystem::path(vaultFolderPath) / filename).replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    int lineCount = 0;
    while(std::getline(file, line)){
        lineCount++;
    }
    return (lineCount == vaultLineNumberOfHash);
}

int CharacterManager::GetFileCountInCharacterDirectory(){
    int res = 0;
    dirItr = std::filesystem::directory_iterator(characterFolderPath);
    for(const auto& entry : dirItr){
        if(entry.is_regular_file()){
            res++;
        }
    }
    return res;
}

int CharacterManager::GetFileCountInVaultDirectory(){
    int res = 0;
    dirItr = std::filesystem::directory_iterator(vaultFolderPath);
    for(const auto& entry : dirItr){
        if(entry.is_regular_file()){
            res++;
        }
    }
    return res;
}

// kills character files of invalid line length or invalid hash
void CharacterManager::KillInvalidCharacterFiles(){
    dirItr = std::filesystem::directory_iterator(characterFolderPath);
    for(auto& file: dirItr){
        std::string fileName = file.path().filename().string();
        if(!CharacterFileHasValidLineCount(fileName) || !ValidateCharacterFileHash(fileName)){
            std::cout << fileName << " was detected as invalid and would be deleted. check the file! exiting! " << std::endl;
            exit(-1); 
            // std::filesystem::remove(file.path());
        }
    }
}

void CharacterManager::CreateNewVaultFile(const std::string& fileName){
    std::string filePath = (std::filesystem::path(vaultFolderPath) / fileName).replace_extension(".txt").string();
    std::ofstream vaultFile(filePath, std::ofstream::trunc);
    vaultFile << "-1,-1,-1,-1,-1,-1,-1,-1" << std::endl;
    vaultFile.close();
    auto hash = GetHashVaultFile(fileName);
    vaultFile.open(filePath, std::ios::app);
    vaultFile << hash;
    vaultFile.close();
}

void CharacterManager::SaveVaults(std::unique_ptr<Registry>& registry){
    KillInvalidVaultFiles();
    const auto& vaultEntities = registry->GetSystem<VaultSystem>().GetSystemEntities();
    for(const auto& vault: vaultEntities){
        std::string currentVaultEntityId = std::to_string(vault.GetComponent<VaultChestComponent>().id);
        dirItr = std::filesystem::directory_iterator(vaultFolderPath);
        for(auto file: dirItr){
            std::string fileName = file.path().filename().string(); //"1", "2", "3"
            fileName = fileName.substr(0,1);
            if(fileName == currentVaultEntityId){
                const auto& items = vault.GetComponent<LootBagComponent>().contents;
                std::vector<int> ie = {-1,-1,-1,-1,-1,-1,-1,-1};
                for(const auto& pair: items){
                    ie[static_cast<int>(pair.first)-1] = static_cast<int>(pair.second.GetComponent<ItemComponent>().itemEnum);
                }
                std::string vaultItemsString = std::to_string(ie[0]) + "," + std::to_string(ie[1]) + "," + std::to_string(ie[2]) + "," + std::to_string(ie[3]) + "," + std::to_string(ie[4]) + "," + std::to_string(ie[5]) + "," + std::to_string(ie[6]) + "," + std::to_string(ie[7]);
                std::string filePath = (std::filesystem::path(vaultFolderPath) / fileName).replace_extension(".txt").string();
                std::ofstream vaultFile(filePath, std::ofstream::trunc);
                vaultFile << vaultItemsString << std::endl;
                vaultFile.close();
                auto hash = GetHashVaultFile(fileName);
                vaultFile.open(filePath, std::ios::app);
                vaultFile << hash;
                vaultFile.close();
            }
        }
    }
}

// ensures we will only have 3 valid vault files
void CharacterManager::KillInvalidVaultFiles(){
    dirItr = std::filesystem::directory_iterator(vaultFolderPath);
    std::set<std::string> filesToKeep;
    // kill files that are of invalid format
    for(auto& file: dirItr){
        std::string fileName = file.path().filename().string();
        if(!VaultFileHasValidLineCount(fileName) || !ValidateVaultFileHash(fileName)){ // delete invalid file
            std::filesystem::remove(file.path());
        } else if(fileName == "1.txt" || fileName == "2.txt" || fileName == "3.txt"){
            filesToKeep.insert(fileName);
        }
    }
    // create missing vault files
    std::vector<std::string> files = {"1.txt", "2.txt", "3.txt"};
    for(auto& x: files){
        if(filesToKeep.find(x) == filesToKeep.end()){
            CreateNewVaultFile(x.substr(0,1));
        }
    }

}

// kills youngest files in character directory until 3 remain regardless of validity
void CharacterManager::KillExcessCharacterFiles(){
    while(GetFileCountInCharacterDirectory() > 3){
        std::filesystem::path youngestFile;
        std::filesystem::file_time_type youngestTime = std::filesystem::file_time_type::min(); // Initialize with the minimum possible time
        for(const auto& file: std::filesystem::directory_iterator(characterFolderPath)){
            if(std::filesystem::is_regular_file(file)){
                std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(file);
                if(fileTime > youngestTime){
                    youngestTime = fileTime;
                    youngestFile = file.path();
                }
            }
        }
        std::filesystem::remove(youngestFile);
    }

}

std::string CharacterManager::GetHashCharacterFile(const std::string& filename) {
    std::string filePath = (std::filesystem::path(characterFolderPath) / filename).replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    std::string message;
    int lineCount = 0;
    while(std::getline(file, line) && lineCount < characterLineNumberOfHash-1) {
        for (char c : line) {
            message += std::to_string((int)c);
        }
        lineCount++;
    }
    std::stringstream numericMessageStream;
    for(char c : message) {
        numericMessageStream << std::to_string((int)c);
    }
    std::string numericMessage = numericMessageStream.str();
    const int chunkSize = 16; 
    std::vector<std::string> chunks;
    for(size_t i = 0; i < numericMessage.length(); i += chunkSize) {
        chunks.push_back(numericMessage.substr(i, chunkSize));
    }
    long long ciphertext = 1;
    for(const std::string& chunk : chunks) {
        long long numericValue = std::stoll(chunk);
        for (int i = 0; i < 3; ++i) {
            ciphertext = (ciphertext * numericValue) % 36093706366919953;
        }
    }
    return std::to_string(ciphertext);
}

std::string CharacterManager::GetHashVaultFile(const std::string& filename) {
    std::string filePath = (std::filesystem::path(vaultFolderPath) / filename).replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    std::string message;
    int lineCount = 0;
    while(std::getline(file, line) && lineCount < vaultLineNumberOfHash-1) {
        for(char c : line) {
            message += std::to_string((int)c);
        }
        lineCount++;
    }
    std::stringstream numericMessageStream;
    for(char c : message) {
        numericMessageStream << std::to_string((int)c);
    }
    std::string numericMessage = numericMessageStream.str();
    const int chunkSize = 16; 
    std::vector<std::string> chunks;
    for(size_t i = 0; i < numericMessage.length(); i += chunkSize) {
        chunks.push_back(numericMessage.substr(i, chunkSize));
    }
    long long ciphertext = 1;
    for(const std::string& chunk : chunks) {
        long long numericValue = std::stoll(chunk);
        for (int i = 0; i < 3; ++i) {
            ciphertext = (ciphertext * numericValue) % 36093706366919953;
        }
    }
    return std::to_string(ciphertext);
}

bool CharacterManager::ValidateCharacterFileHash(const std::string& filename){ // checks if character file has been tampered
    std::string filePath = (std::filesystem::path(characterFolderPath) / filename).replace_extension(".txt").string();
    std::string currentHash = GetHashCharacterFile(filename);
    std::string storedHash;
    int lineCount = 1;
    std::string line;
    std::ifstream file(filePath);
    while(std::getline(file, line)) {
        if(lineCount == characterLineNumberOfHash) {
            storedHash = line;
            break;
        }
        lineCount++;
    }
    // std::cout << "comparing " << storedHash << " as stored with " << currentHash << " as current (" << (storedHash == currentHash) << ")" << std::endl;
    return storedHash == currentHash;
}

bool CharacterManager::ValidateVaultFileHash(const std::string& filename){
    std::string filePath = (std::filesystem::path(vaultFolderPath) / filename).replace_extension(".txt").string();
    std::string currentHash = GetHashVaultFile(filename);
    std::string storedHash;
    int lineCount = 1;
    std::string line;
    std::ifstream file(filePath);
    while(std::getline(file, line)) {
        if(lineCount == vaultLineNumberOfHash) {
            storedHash = line;
            break;
        }
        lineCount++;
    }
    return storedHash == currentHash;
}

std::string CharacterManager::CreateNewCharacterFile(classes className){
    std::int64_t time = static_cast<std::int64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    std::string fileName = std::to_string(time);
    std::string filePath = (std::filesystem::path(characterFolderPath) / fileName).replace_extension(".txt").string();
    std::ofstream characterFile(filePath, std::ofstream::trunc);
    auto stats = classToBaseStats.at(className);
    auto equipment = classesToStartingItems.at(className);
    characterFile << time << std::endl; // line 1 = character primary id (unix timestamp at creation)
    characterFile << className << std::endl; // line 2 = classes enum int
    characterFile << 1 << "," << 0 << std::endl; //line 3 = level, xp 
    //line 4 is for character base stats (hp,mp,att,def,spd,dex,vit,wis)
    characterFile << stats.hp << "," << stats.mp << "," << static_cast<int>(stats.attack) << "," << static_cast<int>(stats.defense) << "," << static_cast<int>(stats.speed) << "," << static_cast<int>(stats.dexterity) << "," << static_cast<int>(stats.vitality) << "," << static_cast<int>(stats.wisdom) << std::endl;
    characterFile << equipment.startingWeapon << "," << equipment.startingEquipment << ",-1,-1" << std::endl; // line 5 = equipment
    characterFile << "-1,-1,-1,-1,-1,-1,-1,-1" << std::endl; // line 6 is inventory contenst (none for new character)
    characterFile.close();
    auto hash = GetHashCharacterFile(fileName);
    characterFile.open(filePath, std::ios::app);
    characterFile << hash; // line 7 is the hash of the first 6 lines!
    characterFile.close();
    return std::to_string(time);
}

void CharacterManager::SaveCharacter(const std::string& activeCharacterID, Entity player){ // not for making a new character; this is for saving progress on an EXISITNG character!
    std::string filePath = (std::filesystem::path(characterFolderPath) / activeCharacterID).replace_extension(".txt").string();
    std::ofstream characterFile(filePath, std::ofstream::trunc);
    const auto& stats = player.GetComponent<BaseStatComponent>();
    const auto& equipment = player.GetComponent<PlayerItemsComponent>().equipment;
    const auto& inventory = player.GetComponent<PlayerItemsComponent>().inventory;
    const auto& className = player.GetComponent<ClassNameComponent>().classname;
    characterFile << activeCharacterID << std::endl;
    characterFile << className << std::endl;
    characterFile << static_cast<int>(stats.level) << "," << stats.xp << std::endl;
    characterFile << stats.hp << "," << stats.mp << "," << static_cast<int>(stats.attack) << "," << static_cast<int>(stats.defense) << "," << static_cast<int>(stats.speed) << "," << static_cast<int>(stats.dexterity) << "," << static_cast<int>(stats.vitality) << "," << static_cast<int>(stats.wisdom) << std::endl;
    for(int i = 1; i < 5; i++){ // equipment  
        equipment.find(i) != equipment.end() ? characterFile << equipment.at(i).GetComponent<ItemComponent>().itemEnum : characterFile << "-1";
        i == 4 ? characterFile << std::endl : characterFile << ",";
    }
    for(int i = 1; i < 9; i++){ // inventory
        inventory.find(i) != inventory.end() ? characterFile << inventory.at(i).GetComponent<ItemComponent>().itemEnum : characterFile << "-1";
        i == 8 ? characterFile << std::endl : characterFile << ",";
    }
    characterFile.close();
    auto hash = GetHashCharacterFile(filePath);
    characterFile.open(filePath, std::ios::app);
    characterFile << hash; // line 7 is the hash of the first 6 lines!
    characterFile.close();

}

std::vector<std::string> CharacterManager::GetAllCharacterValuesAtLineNumber(int linenumber){
    std::vector<std::string> res;
    dirItr = std::filesystem::directory_iterator(characterFolderPath);
    for(auto& file: dirItr){
        std::string fileName = file.path().filename().string();
        std::string filePath = (std::filesystem::path(characterFolderPath) / fileName).replace_extension(".txt").string();
        int lineCount = 1;
        std::string line;
        std::ifstream currentFile(filePath);
        while(std::getline(currentFile, line)){
            if(lineCount == linenumber){
                res.push_back(line);
                break;
            }
        lineCount++;
        }
    }
    return res;
}

// returns a vector of integers copied from data originating at linenumber in fileName
std::vector<int> CharacterManager::GetLineValuesFromCharacterFile(const std::string& fileName, int linenumber){ 
    std::vector<int> res;
    std::string filePath = (std::filesystem::path(characterFolderPath) / fileName).replace_extension(".txt").string();
    int lineCount = 1;
    std::string line;
    std::string word;
    std::ifstream currentFile(filePath);
    while(std::getline(currentFile, line)){
        // std::cout << line << std::endl;
        if(lineCount == linenumber){
            word = "";
            for(auto c: line){
                if(c != ','){
                    word += c;
                } else {
                    res.push_back(stoi(word));
                    word = "";
                }
            }
            res.push_back(stoi(word));
            return res;  
        }
        lineCount++;
    }
    return res;
}

std::vector<int> CharacterManager::GetItemsFromVault(int vaultID){
    std::vector<int> res;
    std::string filePath = (std::filesystem::path(vaultFolderPath) / std::to_string(vaultID)).replace_extension(".txt").string();
    std::string line;
    std::string word = "";
    std::ifstream currentFile(filePath);
    while(std::getline(currentFile, line)){
        for(auto c: line){
            if(c != ','){
                word += c;
            } else {
                res.push_back(stoi(word));
                word = "";
            }
        }
        res.push_back(stoi(word));
        return res;
    }
    return res;
}

void CharacterManager::KillInvalidNameFiles(){
    dirItr = std::filesystem::directory_iterator(nameFolderPath);
    bool nameFileExists;
    for(auto& file: dirItr){
        std::string fileName = file.path().filename().string();
        if(fileName == "name.txt"){
            nameFileExists = true;
        } else {
            std::filesystem::remove(file.path());
        }
    }
    if(!nameFileExists){
        std::string filePath = (std::filesystem::path(nameFolderPath) / "name").replace_extension(".txt").string();
        std::ofstream nameFile(filePath, std::ofstream::trunc);
        nameFile << defaultNames[RNG.randomFromRange(0, defaultNames.size()-1)];
        nameFile.close();
    }
}

void CharacterManager::SaveName(std::string name){
    KillInvalidNameFiles();
    std::string filePath = (std::filesystem::path(nameFolderPath) / "name").replace_extension(".txt").string();
    std::ofstream nameFile(filePath, std::ofstream::trunc);
    nameFile << name;
    nameFile.close();
}

std::string CharacterManager::GetName(){
    KillInvalidNameFiles();
    std::string filePath = (std::filesystem::path(nameFolderPath) / "name").replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    while(std::getline(file, line)){break;}
    std::string name = "";
    for(const auto& c: line){
        if(name.size() == 10){
            return name;
        }
        name.push_back(c);
    }
    return name;

}
