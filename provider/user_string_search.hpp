#ifndef user_string_search_hpp
#define user_string_search_hpp

#include <random>

#include "puzzler/core/puzzle.hpp"
#include "puzzler/puzzles/string_search.hpp"

class StringSearchProvider
  : public puzzler::StringSearchPuzzle
{
public:
  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::StringSearchInput *input,
		       puzzler::StringSearchOutput *output
		       ) const override
  {
    ReferenceExecute(log, input, output);
  }

};

#endif
