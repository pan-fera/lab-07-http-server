// Copyright 2020 pan_fera <your_email>
#ifndef INCLUDE_SUGGESTIONS_HPP_
#define INCLUDE_SUGGESTIONS_HPP_
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class Suggestions {
 public:
  Suggestions();
  void Update(json Json);
  [[nodiscard]] json DoSuggest(const std::string& str) const;

 private:
  json _suggestions;
};

#endif  // INCLUDE_SUGGESTIONS_HPP_
