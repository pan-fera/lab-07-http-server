// Copyright 2020 pan_fera <your_email>
#ifndef INCLUDE_JSONARRAY_HPP_
#define INCLUDE_JSONARRAY_HPP_
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class JsonArray {
 public:
  explicit JsonArray(const std::string& name);
  [[nodiscard]] json GetMemory() const;
  void ReadJson();

 private:
  std::string _name;
  json _memory;
};

#endif  // INCLUDE_JSONARRAY_HPP_
