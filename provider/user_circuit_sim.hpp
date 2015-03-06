#ifndef user_circuit_sim_hpp
#define user_circuit_sim_hpp

#include "puzzler/puzzles/circuit_sim.hpp"


class CircuitSimProvider
  : public puzzler::CircuitSimPuzzle
{
public:
  CircuitSimProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::CircuitSimInput *input,
		       puzzler::CircuitSimOutput *output
		       ) const override {
    ReferenceExecute(log, input, output);
  }

};

#endif
