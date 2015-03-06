#ifndef puzzler_puzzles_life_hpp
#define puzzler_puzzles_life_hpp

#include <random>
#include <sstream>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class LifePuzzle;
  class LifeInput;
  class LifeOutput;

  class LifeInput
    : public Puzzle::Input
  {
  public:

    uint32_t n;
    uint32_t steps;
    std::vector<bool> state;

    LifeInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    LifeInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(steps);
      conn.SendOrRecv(state);
    }



  };

  class LifeOutput
    : public Puzzle::Output
  {
  public:
    std::vector<bool> state;

    LifeOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    LifeOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(state);
    }

    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<LifeOutput>(output);
      return state==pOutput->state;
    }

  };


  class LifePuzzle
    : public PuzzleBase<LifeInput,LifeOutput>
  {
  protected:

    bool update(int n, const std::vector<bool> &curr, int x, int y) const
    {
      int neighbours=0;
      for(int dx=-1;dx<=+1;dx++){
        for(int dy=-1;dy<=+1;dy++){
          int ox=(n+x+dx)%n; // handle wrap-around
          int oy=(n+y+dy)%n;

          if(curr.at(oy*n+ox) && !(dx==0 && dy==0))
            neighbours++;
        }
      }

      if(curr[n*y+x]){
        // alive
        if(neighbours<2){
          return false;
        }else if(neighbours>3){
          return false;
        }else{
          return true;
        }
      }else{
        // dead
        if(neighbours==3){
          return true;
        }else{
          return false;
        }
      }
    }

  protected:
    virtual void Execute(
			 ILog *log,
			 const LifeInput *input,
			 LifeOutput *output
			 ) const =0;

    void ReferenceExecute(
			  ILog *log,
			  const LifeInput *pInput,
			  LifeOutput *pOutput
			  ) const
    {
      log->LogVerbose("About to start running iterations (total = %d)", pInput->steps);

      int n=pInput->n;
      std::vector<bool> state=pInput->state;

      log->Log(Log_Debug, [&](std::ostream &dst){
        dst<<"\n";
        for(unsigned y=0; y<n; y++){
          for(unsigned x=0; x<n; x++){
            dst<<(state.at(y*n+x)?'x':' ');
          }
          dst<<"\n";
        }
      });

      for(unsigned i=0; i<pInput->steps; i++){
        log->LogVerbose("Starting iteration %d of %d\n", i, pInput->steps);

        std::vector<bool> next(n*n);

        for(unsigned x=0; x<n; x++){
          for(unsigned y=0; y<n; y++){
            next[y*n+x]=update(n, state, x, y);
          }
        }

        state=next;

        // The weird form of log is so that there is little overhead
        // if logging is disabled
        log->Log(Log_Debug, [&](std::ostream &dst){
          dst<<"\n";
          for(unsigned y=0; y<n; y++){
            for(unsigned x=0; x<n; x++){
              dst<<(state[y*n+x]?'x':' ');
            }
            dst<<"\n";
          }
        });
      }

      log->LogVerbose("Finished steps");

      pOutput->state=state;
    }

  public:
    virtual std::string Name() const override
    { return "life"; }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog *,
					       int scale
					       ) const override
    {
      std::mt19937 rnd(time(0));  // Not the best way of seeding...

      auto params=std::make_shared<LifeInput>(this, scale);

      params->n=scale;
      params->steps=scale;

      int n=scale;
      params->state.resize(n*n);
      for(unsigned i=0; i<n*n; i++){
        params->state[i]=(rnd()%2)==1;
      }

      return params;
    }

  };

};

#endif
