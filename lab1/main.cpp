#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "RegexRecognizer.h"
#include "SmcRecognizer.h"
#include "FlexRecognizer.h"

void benchmark(const std::string& inputFile, const std::string& outputFile, IMyRecognizer& recognizer) {
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);

    if (!input || !output) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return;
    }

    std::string line;
    long long accumulatedTime = 0;
    int count = 0;
    int c = 20;
    int t = 2000;
    while (std::getline(input, line)) {
            auto start = std::chrono::high_resolution_clock::now();
            recognizer.validate(line);
            auto end = std::chrono::high_resolution_clock::now();

            long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            accumulatedTime += duration;
            if (count%c==0){
        output << count/c << " " << accumulatedTime/c << "\n";
            accumulatedTime=0;}
        count++;
    }

    input.close();
    output.close();
    std::cout << "Файл " << outputFile << " создан." << std::endl;
}

int main() {
    RegexRecognizer regexRecognizer;
    SmcRecognizer smcRecognizer;
    FlexRecognizer flexRecognizer;

    benchmark("correct.txt", "timing_regex.txt", regexRecognizer);
    benchmark("correct.txt", "timing_smc.txt", smcRecognizer);
    benchmark("correct.txt", "timing_flex.txt", flexRecognizer);

    return 0;
}
