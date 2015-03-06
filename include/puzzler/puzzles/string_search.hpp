#ifndef puzzler_puzzles_string_search_hpp
#define puzzler_puzzles_string_search_hpp

#include <random>
#include <regex>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class StringSearchPuzzle;

  class StringSearchInput
    : public Puzzle::Input
  {
  public:
    uint32_t seed;
    std::vector<std::string> patterns;
    uint32_t stringLength;


    StringSearchInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    StringSearchInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }


  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(seed);
      conn.SendOrRecv(patterns);
      conn.SendOrRecv(stringLength);
    }
  };

  class StringSearchOutput
    : public Puzzle::Output
  {
  public:
    std::vector<uint32_t> occurences;


    StringSearchOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    StringSearchOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(occurences);
    }
  public:
    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<StringSearchOutput>(output);
      return occurences==pOutput->occurences;
    }

  };



  class StringSearchPuzzle
    : public PuzzleBase<StringSearchInput,StringSearchOutput>
  {
  protected:

    static uint32_t Step(uint32_t x)
    {
      x=1664525UL * x + 1013904223UL;
      return x;
    }

    template<class TRng>
    std::string MakePattern(uint32_t length, TRng &rng) const
    {
      char bases[8]={'A','C', 'T','G','.'};
      //char inverses[8]={'a', 'c', 't', 'g'};
      std::string acc(length, ' ');
      char prev='z';
      for(unsigned i=0; i<length; i++){
        //acc[i]=(i%2) ? inverses[rng()%4] : bases[rng()%4];
        do{
          acc[i]=bases[rng()%5];
        }while(prev==acc[i]);
        prev=acc[i];
      }
      return acc;
    }

    std::string MakeString(uint32_t length, uint32_t seed) const
    {
      char bases[4]={'A','C','T','G'};

      uint32_t s=seed;
      std::string res(length, ' ');
      for(unsigned i=0; i<length; i++){
	res[i]=bases[(s+(s<<16))>>30];
	s=Step(s);
      }
      return res;
    }

    /*unsigned Matches(const std::string data, unsigned offset, const std::string pattern) const
      {
      unsigned pos=offset;
      unsigned p=0;
      while(p<pattern.size()){
      if(pos>=data.size())
      return 0;

      switch(pattern[p]){
      case 'A':
      case 'C':
      case 'T':
      case 'G':
      if(data[pos]!=pattern[p])
      return 0;
      p++;
      pos++;
      break;
      case 'a':
      case 'c':
      case 't':
      case 'g':
      if(data[pos]==toupper(pattern[p])){
      p++;
      pos++;
      }else{
      pos++;
      }
      break;
      }
      }
      return pos-offset;
      }*/

    unsigned Matches(const std::string data, unsigned offset, const std::string pattern) const
    {
      unsigned pos=offset;
      unsigned p=0;

      int seen=0;
      while(p<pattern.size()){
        if(pos>=data.size())
          return 0;

	//fprintf(stderr, " state=%u, off=%u, seen=%u, d=%c, p=%c", p, pos, seen, data[pos], pattern[p]);
	if(pattern.at(p)=='.'){
	  pos++;
	  p++;
	  seen=0;
	  //fprintf(stderr, " : match any\n");
	}else if(data.at(pos)==pattern.at(p)){
	  seen++;
	  pos++;
	  //fprintf(stderr, " : match exact %s\n", seen==1?"first":"follow");
	}else{
	  if(seen>0){
	    p++;
	    seen=0;
	    // fprintf(stderr, " : move next\n");
	  }else{
	    //fprintf(stderr, "  Fail\n");
	    return 0;
	  }
	}
      };
      return pos-offset;
    }

    virtual void Execute(
			 ILog *log,
			 const StringSearchInput *pInput,
			 StringSearchOutput *pOutput
			 ) const=0;

    void ReferenceExecute(
			  ILog *log,
			  const StringSearchInput *pInput,
			  StringSearchOutput *pOutput
			  ) const
    {
      std::vector<uint32_t> histogram(pInput->patterns.size(), 0);

      std::string data=MakeString(pInput->stringLength, pInput->seed);

      unsigned i=0;
      while(i < pInput->stringLength){

        for(unsigned p=0; p<pInput->patterns.size(); p++){
          unsigned len=Matches(data, i, pInput->patterns[p]);
          if(len>0){
            log->Log(Log_Debug,[&](std::ostream &dst){
		dst<<"  Found "<<pInput->patterns.at(p)<<" at offset "<<i<<", match="<<data.substr(i, len);
              });
            histogram[p]++;
            i += len-1;
            break;
          }
        }

        ++i;
      }

      for(unsigned i=0; i<histogram.size(); i++){
        log->Log(Log_Debug, [&](std::ostream &dst){
	    dst<<pInput->patterns[i].c_str()<<" : "<<histogram[i];
	  });
      }


      pOutput->occurences=histogram;
    }


  public:
    virtual std::string Name() const override
    { return "string_search"; }

    virtual std::shared_ptr<Input> CreateInput
    (
     ILog *,
     int scale
     ) const
    {

      std::mt19937 rnd(time(0));

      std::vector<std::string> patterns;
      for(unsigned i=0; i<(unsigned)sqrt(2+scale); i++){
        patterns.push_back(MakePattern(1+(unsigned)floor(log(scale)/log(10)), rnd));
      }

      auto res=std::make_shared<StringSearchInput>(this, scale);
      res->patterns=patterns;
      res->stringLength=scale;
      return res;
    }




  };

};

#endif
