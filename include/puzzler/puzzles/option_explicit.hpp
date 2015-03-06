#ifndef puzzler_puzzles_option_explicit_hpp
#define puzzler_puzzles_option_explicit_hpp

#include <random>
#include <regex>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class OptionExplicitInput
    : public Puzzle::Input
  {
  public:
    uint32_t n;
    double sigma, r;
    double S0;
    double K;
    double BU;
    double wU, wM, wD;
    double u, d;

    OptionExplicitInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    OptionExplicitInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(sigma).SendOrRecv(r);
      conn.SendOrRecv(S0);
      conn.SendOrRecv(K);
      conn.SendOrRecv(BU);
      conn.SendOrRecv(wU).SendOrRecv(wM).SendOrRecv(wD);
      conn.SendOrRecv(u).SendOrRecv(d);
    }
  };

  class OptionExplicitOutput
    : public Puzzle::Output
  {
  public:
    int steps;
    double value;

    OptionExplicitOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    OptionExplicitOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(steps);
      conn.SendOrRecv(value);
    }
  public:
    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<OptionExplicitOutput>(output);
      return std::abs((value-pOutput->value)/value) < sqrt(steps)*1e-8;
    }

  };

  class OptionExplicitPuzzle
    : public PuzzleBase<OptionExplicitInput,OptionExplicitOutput>
  {
  protected:



    virtual void Execute(
			 ILog *log,
			 const OptionExplicitInput *pInput,
			 OptionExplicitOutput *pOutput
			 ) const=0;

    void ReferenceExecute
    (
     ILog *log,
     const OptionExplicitInput *pInput,
     OptionExplicitOutput *pOutput
     ) const
    {
      int n=pInput->n;
      double u=pInput->u, d=pInput->d;

      log->LogInfo("Params: u=%lg, d=%lg, wU=%lg, wM=%lg, wD=%lg", pInput->u, pInput->d, pInput->wU, pInput->wM, pInput->wD);

      std::vector<double> state(n*2+1);
      double vU=pInput->S0, vD=pInput->S0;
      state[pInput->n]=std::max(vU-pInput->K,0.0);
      for(int i=1; i<=n; i++){
        vU=vU*u;
        vD=vD*d;
        state[n+i]=std::max(vU-pInput->K,0.0);
        state[n-i]=std::max(vD-pInput->K,0.0);
      }

      double wU=pInput->wU, wD=pInput->wD, wM=pInput->wM;
      for(int t=n-1; t>=0; t--){
        std::vector<double> tmp=state;

        vU=pInput->S0, vD=pInput->S0;
        for(int i=0; i<n; i++){
          double vCU=wU*state[n+i+1]+wM*state[n+i]+wD*state[n+i-1];
          double vCD=wU*state[n-i+1]+wM*state[n-i]+wD*state[n-i-1];
          vCU=std::max(vCU, vU-pInput->K);
          vCD=std::max(vCD, vD-pInput->K);
          tmp[n+i]=vCU;
          tmp[n-i]=vCD;

          vU=vU*u;
          vD=vD*d;
        }

        state=tmp;
      }

      pOutput->value=state[n];

      log->LogVerbose("Priced n=%d, S0=%lg, K=%lg, r=%lg, sigma=%lg, BU=%lg : value=%lg", n, pInput->S0, pInput->K, pInput->r, pInput->sigma, pInput->BU, pOutput->value);
    }

  public:
    virtual std::string Name() const override
    { return "option_explicit"; }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog *,
					       int scale
					       ) const
    {
      std::mt19937 rng(time(0));
      std::uniform_real_distribution<> ureal;

      unsigned n=scale;
      double dt=1.0/n;

      double S0=ureal(rng)+0.5;
      double K=S0*(ureal(rng)*0.25+0.825);

      double sigma=ureal(rng)*0.9+0.1;
      double r=ureal(rng)*0.1;

      double u=exp(sigma*sqrt(2*dt));
      double d=1/u;

      double t1=exp(r*dt/2), t2=exp(sigma*sqrt(dt/2));

      double pU=pow((t1-1/t2) /(t2-1/t2),2);
      double pD=pow((t2-t1)   /(t2-1/t2),2);
      double pM=1-pU-pD;

      double discount=exp(-r*dt);

      auto res=std::make_shared<OptionExplicitInput>(this, scale);

      res->n=n;
      res->sigma=sigma;
      res->r=r;
      res->u=u;
      res->d=d;
      res->wU=pU*discount;
      res->wM=pM*discount;
      res->wD=pD*discount;
      res->K=K;
      res->S0=S0;

      return res;
    }


  };

};

#endif
