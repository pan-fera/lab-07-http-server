// Copyright 2020 pan_fera <your_email>
#include "JsonArray.hpp"
#include <exception>
#include <fstream>
#include <iostream>

JsonArray::JsonArray(const std::string& name) : _name(name) {}
[[nodiscard]] json JsonArray::GetMemory() const { return _memory; }
void JsonArray::ReadJson() {
  try {
    std::ifstream in(_name);
    in >> _memory;
  } catch (const std::exception& exc) {
    std::cerr << exc.what() << '\n';
  }
}
