#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <vector>

class UrlGenerator {
private:
    std::mt19937 gen;
    std::uniform_int_distribution<int> letterDist;
    std::uniform_int_distribution<int> lenDist;

public:
    UrlGenerator()
            : gen(std::random_device{}()),
              letterDist(0, 25),
              lenDist(1, 20)
    {}

    char randomLetter() {
        return 'a' + letterDist(gen);
    }

    std::string randomString(int len) {
        std::string s;
        for (int i = 0; i < len; ++i) {
            s += randomLetter();
        }
        return s;
    }

    std::string generateCorrect(int totalLength) {
        if (totalLength < 10) return "";
        std::string url = "http://";
        int remaining = totalLength - 7;

        lenDist.param(std::uniform_int_distribution<int>::param_type(1, std::min(5, remaining - 2)));
        int zoneLen = lenDist(gen);
        remaining -= zoneLen + 1;

        lenDist.param(std::uniform_int_distribution<int>::param_type(1, std::min(20, remaining)));
        int serverLen = lenDist(gen);
        url += randomString(serverLen);
        remaining -= serverLen;

        std::vector<int> domainLengths;
        while (remaining > 1) {
            lenDist.param(std::uniform_int_distribution<int>::param_type(1, std::min(20, remaining - 1)));
            int domainLen = lenDist(gen);
            domainLengths.push_back(domainLen);
            remaining -= domainLen + 1;
        }

        int currentLength = url.length() + (domainLengths.size() > 0 ? domainLengths.size() : 0) + zoneLen + 1;
        if (currentLength < totalLength) {
            int extra = totalLength - currentLength;
            if (!domainLengths.empty()) {
                domainLengths.back() += extra;
            } else {
                serverLen += extra;
                url = "http://" + randomString(serverLen);
            }
        }

        for (int domainLen : domainLengths) {
            url += "." + randomString(domainLen);
        }
        url += "." + randomString(zoneLen);
        return url;
    }

    std::string generateIncorrect(int totalLength) {
        std::string url;
        int type = gen() % 4;

        switch (type) {
            case 0: // Нет "http://"
                url = randomString(totalLength);
                if (url.substr(0, 7) == "http://") url[0] = 'x';
                break;
            case 1: // Слишком длинный сервер (>20)
                url = "http://" + randomString(21 + (gen() % 5)) + "." + randomString(3);
                if (url.length() > totalLength) url = url.substr(0, totalLength);
                else if (url.length() < totalLength) url += randomString(totalLength - url.length());
                break;
            case 2: // Слишком длинная зона (>5)
                url = "http://" + randomString(std::min(20, totalLength - 13)) + ".";
                url += randomString(6 + (gen() % 5));
                if (url.length() > totalLength) url = url.substr(0, totalLength);
                break;
            case 3: // Нет точки или зоны
                url = "http://" + randomString(totalLength - 7);
                size_t dotPos = url.find('.');
                break;
        }
        return url;
    }
};

int main() {
    UrlGenerator generator;
    int maxLength, count;
    std::cout << "Введите максимальную длину строк: ";
    std::cin >> maxLength;
    std::cout << "Введите количество строк для каждого типа и длины: ";
    std::cin >> count;

    std::ofstream correctFile("correct.txt");
    std::ofstream incorrectFile("incorrect.txt");

    if (!correctFile.is_open() || !incorrectFile.is_open()) {
        std::cerr << "Ошибка открытия файлов для записи\n";
        return 1;
    }

    for (int length = 10; length <= maxLength; ++length) {
        for (int i = 0; i < count; ++i) {
            std::string correct = generator.generateCorrect(length);
            if (!correct.empty()) {
                correctFile << correct << "\n";
            }
        }
    }

    for (int length = 10; length <= maxLength; ++length) {
        for (int i = 0; i < count; ++i) {
            std::string incorrect = generator.generateIncorrect(length);
            incorrectFile << incorrect << "\n";
        }
    }

    correctFile.close();
    incorrectFile.close();
    std::cout << "Сгенерированы строки в файлы correct.txt и incorrect.txt\n";
    return 0;
}
