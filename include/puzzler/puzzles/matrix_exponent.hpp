#ifndef puzzler_puzzles_matrix_exponent_hpp
#define puzzler_puzzles_matrix_exponent_hpp

#include <random>
#include <regex>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class MatrixExponentInput
    : public Puzzle::Input
  {
  public:
    uint32_t seed;
    uint32_t n;
    uint32_t steps;

    MatrixExponentInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    MatrixExponentInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(seed);
      conn.SendOrRecv(n);
      conn.SendOrRecv(steps);
    }
  };

  class MatrixExponentOutput
    : public Puzzle::Output
  {
  public:
    std::vector<uint32_t> hashes;

    MatrixExponentOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    MatrixExponentOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(hashes);
    }
  public:
    virtual bool Equals(const Puzzle::Output *output) const override
    {
      auto pOutput=As<MatrixExponentOutput>(output);
      return hashes==pOutput->hashes;
    }

  };


  class MatrixExponentPuzzle
    : public PuzzleBase<MatrixExponentInput,MatrixExponentOutput>
  {
  protected:


    static uint32_t Step(uint32_t x)
    {
      uint64_t tmp=15807ULL*x;
      x=uint32_t(tmp % 2147483647ULL);
      return x;
    }

    static uint32_t Mul(uint32_t a, uint32_t b)
    {
      uint64_t tmp=a*b;
      return uint32_t(tmp%2147483647ULL);
    }

    static uint32_t Add(uint32_t a, uint32_t b)
    {
      uint64_t tmp=a+b;
      return uint32_t(tmp%2147483647ULL);
    }

    static std::vector<uint32_t> MatrixCreate(unsigned n, uint32_t seed)
    {
      std::vector<uint32_t> res(n*n,0);
      for(unsigned r=0; r<n; r++){
        for(unsigned c=0; c<n; c++){
          res[r*n+c]=seed;
          seed=Step(seed);
        }
      }
      return res;
    }

    static std::vector<uint32_t> MatrixIdentity(unsigned n)
    {
      std::vector<uint32_t> res(n*n,0);
      for(unsigned i=0; i<n; i++){
        res[i*n+i]=1;
      }
      return res;
    }

    static std::vector<uint32_t> MatrixMul(unsigned n, std::vector<uint32_t> a, std::vector<uint32_t> b)
    {
      std::vector<uint32_t> res(n*n, 0);

      for(unsigned r=0; r<n; r++){
        for(unsigned c=0; c<n; c++){
          for(unsigned i=0; i<n; i++){
            res[r*n+c] = Add(res[r*n+c], Mul(a[r*n+i], b[i*n+r]));
          }
        }
      }
      return res;
    }

    void ReferenceExecute(
			  ILog *log,
			  const MatrixExponentInput *pInput,
			  MatrixExponentOutput *pOutput
			  ) const override final
    {

      std::vector<uint32_t> hash(pInput->steps);

      log->LogVerbose("Setting up A and identity");
      auto A=MatrixCreate(pInput->n, pInput->seed);
      auto acc=MatrixIdentity(pInput->n);

      log->LogVerbose("Beginning multiplication");
      hash[0]=acc[0];
      for(unsigned i=1; i<pInput->steps; i++){
        log->LogDebug("Iteration %d", i);
        acc=MatrixMul(pInput->n,acc, A);
        hash[i]=acc[0];
      }
      log->LogVerbose("Done");

      pOutput->hashes=hash;
    }

  public:
    virtual std::string Name() const override
    { return "matrix_exponent"; }

    virtual std::shared_ptr<Input> CreateInput
    (
     ILog *,
     int scale
     ) const override
    {
      std::mt19937 rnd(time(0));

      auto res=std::make_shared<MatrixExponentInput>(this, scale);
      res->seed = rnd();
      res->n=scale;
      res->steps=scale;
      return res;
    }



  };

};

#endif
