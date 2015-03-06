#ifndef user_life_hpp
#define user_life_hpp

#include "puzzler/puzzles/life.hpp"


class LifeProvider
  : public puzzler::LifePuzzle
{
public:
  LifeProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::LifeInput *input,
		       puzzler::LifeOutput *output
		       ) const override {
    ReferenceExecute(log, input, output);
  }

};

#endif
