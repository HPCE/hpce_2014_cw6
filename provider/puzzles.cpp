#ifndef local_puzzles_hpp
#define local_puzzles_hpp

#include "user_circuit_sim.hpp"
#include "user_string_search.hpp"
#include "user_matrix_exponent.hpp"
#include "user_option_explicit.hpp"
#include "user_life.hpp"
#include "user_median_bits.hpp"

void puzzler::PuzzleRegistrar::UserRegisterPuzzles()
{
  Register(std::make_shared<CircuitSimProvider>());
  Register(std::make_shared<StringSearchProvider>());
  Register(std::make_shared<MatrixExponentProvider>());
  Register(std::make_shared<OptionExplicitProvider>());
  Register(std::make_shared<LifeProvider>());
  Register(std::make_shared<MedianBitsProvider>());
}


#endif
