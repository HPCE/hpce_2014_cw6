#ifndef puzzler_puzzles_median_bits_hpp
#define puzzler_puzzles_median_bits_hpp

#include <random>
#include <sstream>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class MedianBitsPuzzle;
  class MedianBitsInput;
  class MedianBitsOutput;

  class MedianBitsInput
    : public Puzzle::Input
  {
  public:

    uint32_t n;
    uint32_t seed;

    MedianBitsInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    MedianBitsInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(seed);
    }



  };

  class MedianBitsOutput
    : public Puzzle::Output
  {
  public:
    uint32_t median;

    MedianBitsOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    MedianBitsOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(median);
    }

    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<MedianBitsOutput>(output);
      return median==pOutput->median;
    }

  };


  class MedianBitsPuzzle
    : public PuzzleBase<MedianBitsInput,MedianBitsOutput>
  {
  protected:
    virtual void Execute(
			 ILog *log,
			 const MedianBitsInput *input,
			 MedianBitsOutput *output
			 ) const =0;

    void ReferenceExecute(
			  ILog *pLog,
			  const MedianBitsInput *pInput,
			  MedianBitsOutput *pOutput
			  ) const
    {
      pLog->LogInfo("Generating bits.");
      double tic=now();
      
      std::vector<uint32_t> temp(pInput->n);
      
      for(unsigned i=0; i<pInput->n; i++){
        uint32_t x=i*(7 + pInput->seed);
        uint32_t y=0;
        uint32_t z=0;
        uint32_t w=0;
        
        for(unsigned j=0; j<(unsigned)(log(16+pInput->n)/log(1.1)); j++){
          uint32_t t = x ^ (x << 11);
          x = y; y = z; z = w;
          w = w ^ (w >> 19) ^ t ^ (t >> 8);
        }
        
        temp[i] = w;
      }
      
      pLog->LogInfo("Finding median, delta=%lg", now()-tic);
      tic=now();
      
      std::sort(temp.begin(), temp.end());
      
      pOutput->median=temp[temp.size()/2];  
      
      pLog->LogInfo("Done, median=%u (%lg), delta=%lg", pOutput->median, pOutput->median/pow(2.0,32), now()-tic);
    }

  public:
    virtual std::string Name() const override
    { return "median_bits"; }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog *,
					       int scale
					       ) const override
    {
      std::mt19937 rnd(time(0));  // Not the best way of seeding...

      auto params=std::make_shared<MedianBitsInput>(this, scale);

      params->n=scale;
      params->seed=rnd();
      
      return params;
    }

  };

};

#endif
