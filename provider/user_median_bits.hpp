#ifndef user_median_bits_hpp
#define user_median_bits_hpp

#include "puzzler/puzzles/median_bits.hpp"


class MedianBitsProvider
  : public puzzler::MedianBitsPuzzle
{
public:
  MedianBitsProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::MedianBitsInput *input,
		       puzzler::MedianBitsOutput *output
		       ) const override {
    ReferenceExecute(log, input, output);
  }

};

#endif
