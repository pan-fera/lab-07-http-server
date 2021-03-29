// Copyright 2020 pan_fera <your_email>
#include "Suggestions.hpp"
#include <algorithm>
Suggestions::Suggestions() { _suggestions = {}; }
void Suggestions::Update(json Json) {
  std::sort(Json.begin(), Json.end(), [](const json& a, const json& b) -> bool {
    return a.at("cost") < b.at("cost");
  });
  _suggestions = Json;
}
[[nodiscard]] json Suggestions::DoSuggest(const std::string& str) const{
  json JsonAnswer;
  for (size_t i = 0, m = 0; i < _suggestions.size(); ++i) {
    if (str == _suggestions[i].at("id")) {
      json JsonObject;
      JsonObject["text"] = _suggestions[i].at("name");
      JsonObject["position"] = m++;
      JsonAnswer["suggestions"].push_back(JsonObject);
    }
  }
  return JsonAnswer;
}
