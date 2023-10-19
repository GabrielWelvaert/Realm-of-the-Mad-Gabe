#include "CharacterManager.h"

/*
The CharacterManager is responsible for managing files associated with saved characters
It saves, creates, processes, and validates character files 
*/

CharacterManager::CharacterManager(){}

bool CharacterManager::FileHasValidLineCount(const std::string& filename){
    std::string filePath = (std::filesystem::path(characterFolderPath) / filename).replace_extension(".txt").string();
    std::ifstream file(filePath);
    std::string line;
    int lineCount = 0;
    while(std::getline(file, line)){
        lineCount++;
    }
    return (lineCount == lineNumberOfHash);
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

// kills character files of invalid line length or invalid hash
void CharacterManager::KillInvalidCharacterFiles(){
    dirItr = std::filesystem::directory_iterator(characterFolderPath);
    for(auto& file: dirItr){
        std::string fileName = file.path().filename().string();
        if(!FileHasValidLineCount(fileName) || !ValidateCharacterFile(fileName)){
            std::cout << file.path().c_str() << " was detected as invalid and would be deleted. check the file! exiting! " << std::endl;
            exit(-1); 
            // std::filesystem::remove(file.path());
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
    while(std::getline(file, line) && lineCount < lineNumberOfHash-1) {
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

bool CharacterManager::ValidateCharacterFile(const std::string& filename){ // checks if character file has been tampered
    std::string filePath = (std::filesystem::path(characterFolderPath) / filename).replace_extension(".txt").string();
    std::string currentHash = GetHashCharacterFile(filename);
    std::string storedHash;
    int lineCount = 1;
    std::string line;
    std::ifstream file(filePath);
    while(std::getline(file, line)) {
        if(lineCount == lineNumberOfHash) {
            storedHash = line;
            break;
        }
        lineCount++;
    }
    // std::cout << "comparing " << storedHash << " as stored with " << currentHash << " as current (" << (storedHash == currentHash) << ")" << std::endl;
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
