#include <cstdio>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Trie {
   public:
    Trie() { nodes_.emplace_back(); }

    std::string_view GetCurWord() const { return cur_word_; }

    void AddWord(std::string_view word) {
      cur_pos_ = 0;
      cur_word_.clear();
      MoveForward(word, true);
      ++nodes_[cur_pos_].words_count_;
      auto idx = cur_pos_;
      nodes_[idx].max_words_count =
            std::max(nodes_[idx].max_words_count, nodes_[idx].words_count_);
      while (nodes_[idx].parent_ != -1) {
        const auto parent = nodes_[idx].parent_;
        nodes_[parent].max_words_count =
            std::max(nodes_[parent].max_words_count, nodes_[idx].max_words_count);
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
        std::uint32_t sons_max = 0;
        std::uint32_t idx_max = -1;
        char max_letter;
        for (const auto& [letter, son] : cur_node.next_) {
          const auto& son_node = nodes_[son];
          if (son_node.max_words_count > sons_max) {
            sons_max = son_node.max_words_count;
            idx_max = son;
            max_letter = letter;
          }
        }
        if (idx_max == -1) {
          return cur_node.words_count_ > 0 ? suggestion : "";
        }
        if (cur_node.words_count_ >= sons_max) {
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
      std::int32_t max_words_count = 0;
    };

    std::vector<Node> nodes_;
    std::string cur_word_;
    std::int32_t cur_pos_ = 0;
  };