#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <string>
#include "IMyRecognizer.h"
#include "RegexRecognizer.h"
#include "SmcRecognizer.h"
#include "FlexRecognizer.h"

void processInputManual(IMyRecognizer &recognizer) {
    std::unordered_map<std::string, int> serverStats;
    std::string url;
    while (true) {
        std::cout << "Введите URL (или 'exit' для выхода): ";
        std::getline(std::cin, url);
        if (url == "exit")
            break;
        bool valid = recognizer.validate(url);
        std::cout <<  (valid ? "Valid" : "Invalid") << std::endl;
    }
}

void processInputFile(IMyRecognizer& recognizer, const std::string& inputFile, const std::string& outputFile) {
    std::unordered_map<std::string, int> serverStats;

    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть входной файл '" << inputFile << "'\n";
        return;
    }

    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть выходной файл '" << outputFile << "'\n";
        inFile.close();
        return;
    }

    std::string url;
    int lineCount = 0;
    while (std::getline(inFile, url)) {
        lineCount++;
        if (url.empty()) continue;
        bool valid = recognizer.validate(url);
        outFile << url << ": " << (valid ? "Valid" : "Invalid") << "\n";

        inFile.close();
        outFile.close();
    }
}

void printServerStats(const IMyRecognizer& recognizer) {
    const auto& stats = recognizer.getServerStats();
    if (stats.empty()) {
        std::cout << "Нет данных о серверах.\n";
    } else {
        std::cout << "Статистика по серверам:\n";
        for (const auto& [server, count] : stats) {
            std::cout << "Сервер: " << server << " - Количество: " << count << "\n";
        }
    }
}


int main(int argc, char* argv[]) {
    int choice;
    std::cout << "Выберите способ ввода:\n"
              << "1 - Ввести URL вручную\n"
              << "2 - Проверить URL из файла\n"
              << "Введите ваш выбор: ";
    std::cin >> choice;
    std::cin.ignore();

    std::unique_ptr<IMyRecognizer> recognizer;
    std::string method = (argc > 1) ? argv[1] : "smc";
    if (method == "regex") {
        recognizer = std::make_unique<RegexRecognizer>();
    } else if (method == "smc") {
        recognizer = std::make_unique<SmcRecognizer>();
    } else if (method == "flex") {
        recognizer = std::make_unique<FlexRecognizer>();
    } else {
        std::cerr << "Неизвестный метод: " << method << "\n";
        return 1;
    }

    if (choice == 1) {
        processInputManual(*recognizer);
    } else if (choice == 2) {
        std::string inputFile, outputFile;
        std::cout << "Введите имя входного файла: ";
        std::getline(std::cin, inputFile);
        std::cout << "Введите имя выходного файла: ";
        std::getline(std::cin, outputFile);
        processInputFile(*recognizer, inputFile, outputFile);
    } else {
        std::cerr << "Некорректный выбор!" << std::endl;
    }
    printServerStats(*recognizer);
    return 0;
}