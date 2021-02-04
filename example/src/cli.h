/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _CLI_H_
#define _CLI_H_
#include <string>
#include <vector>

class CLI {
 public:
  CLI(int& argc, char** argv) {
    for (auto i = 1; i < argc; ++i)
      tokens_.push_back(std::string(argv[i]));
  }

  const std::string& Get(const std::string &option) const {
    auto itr =  std::find(tokens_.begin(), tokens_.end(), option);
    if (itr != tokens_.end() && ++itr != tokens_.end()){
      return *itr;
    }
    static const std::string empty_string("");
    return empty_string;
  }

  bool Defined(const std::string &option) const {
    return std::find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
  }

 private:
  std::vector <std::string> tokens_;
};

#endif  // ifndef _CLI_H_
