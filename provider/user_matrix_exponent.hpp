#ifndef user_matrix_exponent_hpp
#define user_matrix_exponent_hpp

#include "puzzler/puzzles/matrix_exponent.hpp"

class MatrixExponentProvider
  : public puzzler::MatrixExponentPuzzle
{
public:
  MatrixExponentProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::MatrixExponentInput *input,
		       puzzler::MatrixExponentOutput *output
		       ) const override {
    return ReferenceExecute(log, input, output);
  }

};

#endif
