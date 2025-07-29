#include <iostream>
#include <string>
#include <memory>
#include "../ast/ast.h"
#include "builder.h"
#include "../nfa/NFA.h"
#include "../dfa/DFA.h"
#include "../my_regex/my_regex.h"
#include <gtest/gtest.h>

using namespace regex;

TEST(check_match, Subtest_1)
{
    Regex regex("c(a|b)+");
    ASSERT_FALSE(regex.compile() );
    if (regex.match("caba")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_match, Subtest_2)
{
    Regex regex("a*");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_match, Subtest_0)
{
    Regex regex("a*");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("b")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_match, Subtest_3)
{
    Regex regex("a[a-c]");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("ab")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_match, Subtest_4)
{
    Regex regex("a{2,3}");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("aaa")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}

TEST(check_match, Subtest_5)
{
    Regex regex("$");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_match, Subtest_6)
{
    Regex regex("a/b");
    ASSERT_TRUE(regex.compile() == 0);
    if (regex.match("ab")) {
        std::cout << "Строка подходит!\n";
    } else {
        std::cout << "Строка не подходит.\n";
    }
}
TEST(check_search, Subtest_1)
{
    regex::Regex re("abz");
    re.compile();
    std::string text = "zzababzz";
    auto match = re.search(text);
    if (match) {
        std::cout << "Match found at: " << match->getMatch(0).first
                  << " to " << match->getMatch(0).second << "\n";
        std::cout << "Matched substring: " << text.substr(match->getMatch(0).first,
                                                          match->getMatch(0).second - match->getMatch(0).first) << "\n";
    } else {
        std::cout << "No match found.\n";
    }
}
TEST(check_search, Union)
{
    regex::Regex re("a|b");
    re.compile();

    std::cout << "\nTesting Union:\n";

    std::string text1 = "abc";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "bcd";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }

    std::string text3 = "cab";
    auto match3 = re.search(text3);
    if (match3) {
        std::cout << "Match in '" << text3 << "' at: " << match3->getMatch(0).first
                  << " to " << match3->getMatch(0).second
                  << " (" << text3.substr(match3->getMatch(0).first,
                                          match3->getMatch(0).second - match3->getMatch(0).first)
                  << ")\n";
    }
}

TEST(check_search, PositiveClosure)
{
    regex::Regex re("(ab)+");
    re.compile();

    std::cout << "\nTesting PositiveClosure:\n";

    std::string text1 = "ababx";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "zab";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }
}

TEST(check_search, Range2to3)
{
    regex::Regex re("a{2,3}");
    re.compile();

    std::cout << "\nTesting Range2to3:\n";

    std::string text1 = "aaaa";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "aaaaa";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }
}

TEST(check_search, Complex)
{
    regex::Regex re("(a|b)+c");
    re.compile();

    std::cout << "\nTesting Complex:\n";

    std::string text1 = "ababc";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "abcabc";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }
}

TEST(check_search, SubstringUnion)
{
    regex::Regex re("ab|bc");
    re.compile();

    std::cout << "\nTesting SubstringUnion:\n";

    std::string text1 = "abcbc";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "bab";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }
}

TEST(check_search, Exact3)
{
    regex::Regex re("a{3}");
    re.compile();

    std::cout << "\nTesting Exact3:\n";

    std::string text1 = "aaaa";
    auto match1 = re.search(text1);
    if (match1) {
        std::cout << "Match in '" << text1 << "' at: " << match1->getMatch(0).first
                  << " to " << match1->getMatch(0).second
                  << " (" << text1.substr(match1->getMatch(0).first,
                                          match1->getMatch(0).second - match1->getMatch(0).first)
                  << ")\n";
    }

    std::string text2 = "aaa";
    auto match2 = re.search(text2);
    if (match2) {
        std::cout << "Match in '" << text2 << "' at: " << match2->getMatch(0).first
                  << " to " << match2->getMatch(0).second
                  << " (" << text2.substr(match2->getMatch(0).first,
                                          match2->getMatch(0).second - match2->getMatch(0).first)
                  << ")\n";
    }
}
void checkRecoverSameBehavior(const std::string& pattern, const std::vector<std::string>& samples) {
    regex::Regex original(pattern);
    original.compile();

    std::string recoveredPattern = original.recover();
    regex::Regex recovered(recoveredPattern);
    recovered.compile();

    for (const std::string& input : samples) {
        bool originalResult = original.match(input);
        bool recoveredResult = recovered.match(input);

        EXPECT_EQ(originalResult, recoveredResult)
                            << "Mismatch on input: \"" << input << "\"\n"
                            << "  Original pattern:   " << pattern << "\n"
                            << "  Recovered pattern:  " << recoveredPattern << "\n";
    }
}
TEST(recover_equivalence, bounded_repetition) {
    checkRecoverSameBehavior("a{2,3}", {
            "", "a", "aa", "aaa", "aaaa", "aaaaa", "b"
    });
}
TEST(recover_equivalence, nested_alternation) {
    checkRecoverSameBehavior("(foo|bar)", {
            "", "foo", "bar", "baz", "foobar"
    });
}
TEST(recover_equivalence, nested_plus) {
    checkRecoverSameBehavior("(foo)+", {
            "", "foo", "bar", "foofoo", "foofoofoo", "fooo"
    });
}
TEST(recover_equivalence, nested_kleene) {
    checkRecoverSameBehavior("(foo)*", {
            "foo", "bar", "foofoo", "foofoofoo", "fooo"
    });
}
TEST(RecoverTest, FooBarBaz)
{
    // Оригинальное регулярное выражение
    regex::Regex originalReg("(foo|bar|baz)+");
    originalReg.compile();

    // Восстановленное регулярное выражение
    std::string recovered = originalReg.recover();
    regex::Regex recoveredReg(recovered);
    recoveredReg.compile();

    // Входная строка для тестирования
    std::string text = "bazz";

    // Проверка, что оба регулярных выражения совпадают с текстом
    bool originalResult = originalReg.match(text);
    bool recoveredResult = recoveredReg.match(text);

    // Ожидаем, что оба регулярных выражения дадут одинаковый результат
    EXPECT_EQ(originalResult, recoveredResult);
}
TEST(check_raznost, BasicDifference)
{
    // Создаем два регулярных выражения
    Regex regexA("a+b*");  // Регулярное выражение для строк вида 'a' и 'b'
    Regex regexB("ab");    // Регулярное выражение для строк 'ab'

    ASSERT_TRUE(regexA.compile() == 0);  // Компилируем регулярные выражения
    ASSERT_TRUE(regexB.compile() == 0);

    // Получаем разность двух регулярных выражений
    Regex difference = regexA.raznost(regexB);
    // Проверка на строки, которые должны попасть в разность
    std::cout << "\nTesting Difference between a+b* and ab:\n";

    if (difference.match("a")) {
        std::cout << "Строка 'a' подходит для разности!\n";
    } else {
        std::cout << "Строка 'a' не подходит для разности.\n";
    }

    if (difference.match("b")) {
        std::cout << "Строка 'b' подходит для разности!\n";
    } else {
        std::cout << "Строка 'b' не подходит для разности.\n";
    }

    if (difference.match("aa")) {
        std::cout << "Строка 'aa' подходит для разности!\n";
    } else {
        std::cout << "Строка 'aa' не подходит для разности.\n";
    }

    if (difference.match("ab")) {
        std::cout << "Строка 'ab' подходит для разности!\n";
    } else {
        std::cout << "Строка 'ab' не подходит для разности.\n";
    }

    if (difference.match("aab")) {
        std::cout << "Строка 'aab' подходит для разности!\n";
    } else {
        std::cout << "Строка 'aab' не подходит для разности.\n";
    }

    if (difference.match("abb")) {
        std::cout << "Строка 'abb' подходит для разности!\n";
    } else {
        std::cout << "Строка 'abb' не подходит для разности.\n";
    }
}
TEST(check_raznost, CompleteOverlapCase)
{
    Regex regexA("a.b");
    Regex regexB("a.b");

    ASSERT_TRUE(regexA.compile() == 0);
    ASSERT_TRUE(regexB.compile() == 0);

    Regex difference = regexA.raznost(regexB);
    std::cout << "\nTesting Difference between a.b and a.b (should be empty):\n";

    // Разность должна быть пустой
    if (difference.match("axb")) {
        std::cout << "Строка 'axb' подходит для разности (ошибка)!\n";
    } else {
        std::cout << "Строка 'axb' не подходит для разности (корректно)!\n";
    }

    if (difference.match("a b")) {
        std::cout << "Строка 'a b' подходит для разности (ошибка)!\n";
    } else {
        std::cout << "Строка 'a b' не подходит для разности (корректно)!\n";
    }
}

TEST(check_raznost, EmptyStringCases2)
{
    Regex regexA("a+");          // Пустая строка или любое количество 'a'
    Regex regexB("b");          // Любое количество 'a', но не пустая строка

    ASSERT_TRUE(regexA.compile() == 0);
    ASSERT_TRUE(regexB.compile() == 0);

    Regex difference = regexA.raznost(regexB);
    std::cout << "\nTesting Difference between a* and a+ (should match only empty string):\n";

    if (difference.match("")) {
        std::cout << "Пустая строка подходит для разности (не ок)!\n";
    } else {
        std::cout << "Пустая строка не подходит для разности (ок)!\n";
    }

    if (difference.match("aa")) {
        std::cout << "Строка 'aa' подходит для разности (ок)!\n";
    } else {
        std::cout << "Строка 'aa' не подходит для разности (некорректно)!\n";
    }
}

TEST(check_complement, BasicComplement)
{
    Regex regexA("(ab)+");
    ASSERT_TRUE(regexA.compile() == 0);

    Regex complementA = regexA.complement();
    std::cout << "\nTesting Complement for a+ (should match all except a, aa, aaa...):\n";

    // Не должны совпадать с исходным regexA
    EXPECT_FALSE(complementA.match("ab")) << "Строка 'a' не должна совпадать с дополнением";
    EXPECT_FALSE(complementA.match("abababab")) << "Строка 'aaaa' не должна совпадать с дополнением";

    // Должны совпадать с дополнением
    EXPECT_TRUE(complementA.match("")) << "Пустая строка должна быть в дополнении";
    EXPECT_TRUE(complementA.match("b")) << "Строка 'b' должна быть в дополнении";
    EXPECT_TRUE(complementA.match("aa")) << "Строка 'ab' должна быть в дополнении";
}

TEST(check_complement, FullLanguageComplement)
{
    Regex regexA(".*"); // Все возможные строки
    ASSERT_TRUE(regexA.compile() == 0);

    Regex complementA = regexA.complement();
    std::cout << "\nTesting Complement for .* (should match nothing):\n";

    // Дополнение должно быть пустым
    EXPECT_FALSE(complementA.match("")) << "Пустая строка не должна совпадать";
    EXPECT_FALSE(complementA.match("a")) << "Строка 'a' не должна совпадать";
    EXPECT_FALSE(complementA.match("anything")) << "Любая строка не должна совпадать";
}

TEST(check_raznost, EmptyStringCases) {
    Regex regexA("a*");          // Пустая строка или любое количество 'a'
    Regex regexB("a+");          // Любое количество 'a', но не пустая строка

    ASSERT_TRUE(regexA.compile() == 0);
    ASSERT_TRUE(regexB.compile() == 0);

    Regex difference = regexA.raznost(regexB);
    std::cout << "\nTesting Difference between a* and a+ (should match only empty string):\n";

    if (difference.match("")) {
        std::cout << "Пустая строка подходит для разности (корректно)!\n";
    } else {
        std::cout << "Пустая строка не подходит для разности (ошибка)!\n";
    }

    if (difference.match("a")) {
        std::cout << "Строка 'a' подходит для разности (ошибка)!\n";
    } else {
        std::cout << "Строка 'a' не подходит для разности (корректно)!\n";
    }
}

TEST(check_match, EmptyStringCases) {
    Regex regex("a+b*");           // Любое количество 'a', но не пустая строка

    ASSERT_TRUE(regex.compile() == 0);

    std::string testString = "aaabbb";
    auto result = regex.search(testString);
    if (result) {
        std::cout << "Found match: " << result->getMatchedString() << std::endl;
        std::cout << "Position: " << result->getMatch().first << "-"
                  << result->getMatch().second << std::endl;
    }

    regex::MatchResult matchResult;
    if (regex.match(testString, matchResult)) {
        std::cout << "Full match: " << matchResult.getMatchedString() << std::endl;
    }
}