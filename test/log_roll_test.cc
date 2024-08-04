#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

class Logger {
public:
    Logger(const std::string& baseFileName, std::size_t maxFileSize, std::size_t maxFileCount)
        : baseFileName(baseFileName), maxFileSize(maxFileSize), maxFileCount(maxFileCount), currentFileIndex(0) {
        rollOver();
    }

    void log(const std::string& message) {
        std::ofstream outFile;
        outFile.open(getCurrentFileName(), std::ios_base::app);
        if (outFile.tellp() >= maxFileSize) {
            rollOver();
            outFile.open(getCurrentFileName(), std::ios_base::app);
        }
        outFile << message << std::endl;
    }

private:
    std::string baseFileName;
    std::size_t maxFileSize;
    std::size_t maxFileCount;
    std::size_t currentFileIndex;

    std::string getCurrentFileName() {
        return baseFileName + std::to_string(currentFileIndex) + ".log";
    }

    void rollOver() {
        currentFileIndex = (currentFileIndex + 1) % maxFileCount;
        std::string newFileName = getCurrentFileName();
        std::ofstream outFile(newFileName, std::ios::trunc); // Create a new empty file
    }
};

int main() {
    Logger logger("logfile", 1024, 5); // Each file max size 1KB, keep 5 files max

    for (int i = 0; i < 100; ++i) {
        logger.log("This is a log message " + std::to_string(i));
    }

    return 0;
}
