#include <algorithm>
#include <cstdio>
#include <deque>
#include <format>
#include <iostream>
#include <queue>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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
          auto last_word = std::string(trie.GetCurWord());
          trie.MoveForward(splitted_input[1], false);
          std::cout << std::format("Appended, suggesting for {}",
                                   last_word + std::string(splitted_input[1]))
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

  class Trie {
   public:
    Trie() { nodes_.emplace_back(); }

    std::string_view GetCurWord() const { return cur_word_; }

    void AddWord(std::string_view word) {
      cur_pos_ = 0;
      MoveForward(word, true);
      ++nodes_[cur_pos_].words_count_;
      auto idx = cur_pos_;
      while (idx != -1 && nodes_[idx].parent_ != -1) {
        nodes_[idx].max_word_count =
            std::max(nodes_[idx].max_word_count, nodes_[idx].words_count_);
        const auto parent = nodes_[idx].parent_;
        nodes_[parent].max_word_count =
            std::max(nodes_[parent].max_word_count, nodes_[idx].max_word_count);
        idx = parent;
      }
    }

    void MatchPrefix(std::string_view word) {
      cur_pos_ = 0;
      cur_word_.clear();
      MoveForward(word, false);
    }

    void MoveForward(std::string_view word, bool push) {
      for (const auto& letter : word) {
        if (!nodes_[cur_pos_].next_.contains(letter)) {
          if (!push) {
            cur_pos_ = -1;
            break;
          }
          nodes_[cur_pos_].next_[letter] = nodes_.size();
          nodes_.emplace_back();
          nodes_.back().parent_ = cur_pos_;
        }
        cur_pos_ = nodes_[cur_pos_].next_[letter];
      }
      cur_word_ += std::string(word);
    }

    std::string MakeSuggestion() {
      if (cur_pos_ == -1) {
        return "";
      }
      std::string suggestion = cur_word_;
      std::int32_t suggestion_pos = cur_pos_;
      while (true) {
        const auto& cur_node = nodes_[suggestion_pos];
        bool is_terminate = cur_node.words_count_ > 0;
        std::uint32_t sons_max = 0;
        std::uint32_t idx_max = -1;
        char max_letter;
        for (const auto& [letter, son] : cur_node.next_) {
          const auto& son_node = nodes_[son];
          if (son_node.max_word_count > sons_max) {
            sons_max = son_node.max_word_count;
            idx_max = son;
            max_letter = letter;
          }
        }
        if (idx_max == -1) {
          return is_terminate ? suggestion : std::string();
        }
        if (is_terminate && cur_node.max_word_count >= sons_max) {
          return suggestion;
        }
        suggestion += max_letter;
        suggestion_pos = idx_max;
      }
    }

   private:
    struct Node {
      std::unordered_map<char, std::int32_t> next_;
      std::int32_t parent_ = -1;
      std::int32_t words_count_ = 0;
      std::int32_t max_word_count = 0;
    };

    std::vector<Node> nodes_;
    std::string cur_word_;
    std::int32_t cur_pos_ = 0;
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