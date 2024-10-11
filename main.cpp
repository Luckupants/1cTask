#include <algorithm>
#include <cstdio>
#include <deque>
#include <format>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

#include "trie.hpp"

class App {
 public:
  void RunApp() {
    while (true) {
      std::string input;
      std::getline(std::cin, input);
      auto splitted_input = SplitInput(input);
      switch (ParseInput(splitted_input)) {
        case ParseResult::kEmpty: {
          std::cout << "Empty input" << std::endl;
          break;
        }
        case ParseResult::kUnknown: {
          std::cout << "Wrong input, try again" << std::endl;
          break;
        }
        case ParseResult::kAddWords: {
          for (const auto& word : splitted_input | std::views::drop(1)) {
            trie.AddWord(word);
          }
          break;
        }
        case ParseResult::kAppendix: {
          trie.MoveForward(splitted_input[1], false);
          std::cout << std::format("Appended, suggesting for {}",
                                   trie.GetCurWord())
                    << std::endl;
          ShowResult();
          break;
        }
        case ParseResult::kNewWord: {
          trie.MatchPrefix(splitted_input[0]);
          ShowResult();
          break;
        }
        case ParseResult::kTerminate: {
          return;
        }
      }
    }
  }

 private:
  enum class ParseResult {
    kEmpty,
    kAddWords,
    kNewWord,
    kAppendix,
    kUnknown,
    kTerminate,
  };

  ParseResult ParseInput(const std::vector<std::string_view>& splitted_input) {
    if (splitted_input.empty()) {
      return ParseResult::kEmpty;
    }
    if (splitted_input[0] == "++") {
      return splitted_input.size() > 1 ? ParseResult::kAddWords
                                       : ParseResult::kUnknown;
    }
    if (splitted_input[0] == "+") {
      return splitted_input.size() == 2 ? ParseResult::kAppendix
                                        : ParseResult::kUnknown;
    }
    if (splitted_input.size() >= 2) {
      return ParseResult::kUnknown;
    }
    if (splitted_input[0] == "exit") {
      return ParseResult::kTerminate;
    }
    return ParseResult::kNewWord;
  }

  std::vector<std::string_view> SplitInput(std::string_view input) {
    auto splitted_input = std::views::split(input, ' ');
    std::vector<std::string_view> words(splitted_input.begin(),
                                        splitted_input.end());
    const auto removed = std::ranges::remove_if(
        words, [](std::string_view word) { return word.empty(); });
    words.resize(removed.begin() - words.begin());
    return words;
  }

  void ShowResult() {
    auto suggestion = trie.MakeSuggestion();
    if (suggestion.empty()) {
      std::cout << "There are no such words" << std::endl;
      return;
    }
    std::cout << std::format("Suggestion: {}", suggestion) << std::endl;
  }

  Trie trie;
};

int main() {
  App app;
  app.RunApp();
}