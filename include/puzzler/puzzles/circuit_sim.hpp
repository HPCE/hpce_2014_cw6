#ifndef puzzler_puzzles_circuit_sim_hpp
#define puzzler_puzzles_circuit_sim_hpp

#include <random>
#include <sstream>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class CircuitSimPuzzle;
  class CircuitSimInput;
  class CircuitSimOutput;

  class CircuitSimInput
    : public Puzzle::Input
  {
  public:

    uint32_t nandGateCount;
    uint32_t flipFlopCount;

    // A list of pairs (src1,src2). If src<0 it refers to a flip-flop. If src>0 it refers to a nand output
    std::vector<std::pair<int32_t,int32_t> > nandGateInputs;

    // A list of srcs. If src<0 it refers to a flip-flop. If src>0 it refers to a nand output
    std::vector<int32_t> flipFlopInputs;


    uint32_t clockCycles;
    std::vector<bool> inputState;


    CircuitSimInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    CircuitSimInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(nandGateCount).SendOrRecv(flipFlopCount);
      conn.SendOrRecv(nandGateInputs).SendOrRecv(flipFlopInputs);
      conn.SendOrRecv(inputState);
      conn.SendOrRecv(clockCycles);


      if(nandGateCount!=nandGateInputs.size())
        throw std::runtime_error("CircuitSimInput::Persist - nandGateCount is inconsistent.");
      if(flipFlopCount!=flipFlopInputs.size())
        throw std::runtime_error("CircuitSimInput::Persist - flipFlopCount is inconsistent.");
      if(inputState.size()!=flipFlopInputs.size())
        throw std::runtime_error("CircuitSimInput::Persist - state size is inconsistent.");
    }



  };

  class CircuitSimOutput
    : public Puzzle::Output
  {
  public:
    std::vector<bool> outputState;

    CircuitSimOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    CircuitSimOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(outputState);
    }

    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<CircuitSimOutput>(output);
      return outputState==pOutput->outputState;
    }

  };


  class CircuitSimPuzzle
    : public PuzzleBase<CircuitSimInput,CircuitSimOutput>
  {
  protected:

    bool calcSrc(unsigned src, const std::vector<bool> &state, const CircuitSimInput *input) const
    {
      if(src < input->flipFlopCount){
	return state.at(src);
      }else{
	unsigned nandSrc=src - input->flipFlopCount;
	bool a=calcSrc(input->nandGateInputs.at(nandSrc).first, state, input);
	bool b=calcSrc(input->nandGateInputs.at(nandSrc).second, state, input);
	return !(a&&b);
      }
    }

    std::vector<bool> next(const std::vector<bool> &state, const CircuitSimInput *input) const
    {
      std::vector<bool> res(state.size());
      for(unsigned i=0; i<res.size(); i++){
	res[i]=calcSrc(input->flipFlopInputs[i], state, input);
      }
      return res;
    }

  protected:

    virtual void Execute(
			 ILog *log,
			 const CircuitSimInput *input,
			 CircuitSimOutput *output
			 ) const =0;

    void ReferenceExecute(
			  ILog *log,
			  const CircuitSimInput *pInput,
			  CircuitSimOutput *pOutput
			  ) const
    {
      log->LogVerbose("About to start running clock cycles (total = %d", pInput->clockCycles);
      std::vector<bool> state=pInput->inputState;
      for(unsigned i=0; i<pInput->clockCycles; i++){
	log->LogVerbose("Starting iteration %d of %d\n", i, pInput->clockCycles);

	state=next(state, pInput);

	// The weird form of log is so that there is little overhead
	// if logging is disabled
	log->Log(Log_Debug,[&](std::ostream &dst) {
	    for(unsigned i=0; i<state.size(); i++){
	      dst<<state[i];
	    }
	  });
      }

      log->LogVerbose("Finished clock cycles");

      pOutput->outputState=state;
    }

  public:
    virtual std::string Name() const override
    { return "circuit_sim"; }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog *,
					       int scale
					       ) const override
    {
      std::mt19937 rnd(time(0));  // Not the best way of seeding...

      auto params=std::make_shared<CircuitSimInput>(this, scale);

      params->clockCycles=scale;
      params->flipFlopCount=scale;
      params->nandGateCount=10*scale;

      params->nandGateInputs.resize(params->nandGateCount);
      params->flipFlopInputs.resize(params->flipFlopCount);

      std::vector<unsigned> todo;
      std::vector<unsigned> done;

      for(unsigned i=0; i<params->flipFlopCount; i++){
	done.push_back(i);
      }
      for(unsigned i=0; i<params->nandGateCount; i++){
	todo.push_back(i+params->flipFlopCount);
      }

      while(todo.size()>0){
	unsigned idx=rnd()%todo.size();
	unsigned curr=todo[idx];
	todo.erase(todo.begin()+idx);

	unsigned currNand=curr - params->flipFlopCount;

	unsigned src1=done[rnd()%done.size()];
	unsigned src2=done[rnd()%done.size()];

	params->nandGateInputs[currNand].first=src1;
	params->nandGateInputs[currNand].second=src2;

	done.push_back(curr);
      }

      for(unsigned i=0; i<params->flipFlopCount; i++){
	params->flipFlopInputs[i]=done[rnd()%done.size()];
      }

      params->inputState.resize(params->flipFlopCount);
      for(unsigned i=0; i<params->flipFlopCount; i++){
	params->inputState[i] = 1 == (rnd()&1);
      }

      return params;
    }

  };

};

#endif
