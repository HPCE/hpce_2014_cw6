#ifndef user_option_explicit_hpp
#define user_option_explicit_hpp

#include <random>

#include "puzzler/core/puzzle.hpp"
#include "puzzler/puzzles/option_explicit.hpp"

class OptionExplicitProvider
  : public puzzler::OptionExplicitPuzzle
{
public:
  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::OptionExplicitInput *input,
		       puzzler::OptionExplicitOutput *output
		       ) const override {
    ReferenceExecute(log, input, output);
  }

};

#endif
