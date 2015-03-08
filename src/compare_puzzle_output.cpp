
#include "puzzler/puzzler.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
   puzzler::PuzzleRegistrar::UserRegisterPuzzles();

   if(argc<2){
      fprintf(stderr, "compare_puzzle_output ref got logLevel\n");
      std::cout<<"Puzzles:\n";
      puzzler::PuzzleRegistrar::ListPuzzles();
      exit(1);
   }

   try{
      std::string refName=argv[1];
      std::string gotName=argv[2];

      // Control how much is being output.
      // Higher numbers give you more info
      int logLevel=atoi(argv[3]);
      fprintf(stderr, "LogLevel = %s -> %d\n", argv[3], logLevel);
      
      std::shared_ptr<puzzler::ILog> logDest=std::make_shared<puzzler::LogDest>("execute_puzzle", logLevel);
      logDest->Log(puzzler::Log_Info, "Created log.");

      logDest->LogInfo("Loading reference %s", refName.c_str());
      std::shared_ptr<puzzler::Puzzle::Output> ref;
      {
         puzzler::FileInStream src(refName);
         puzzler::PersistContext ctxt(&src, false);

         ref=puzzler::PuzzleRegistrar().LoadOutput(ctxt);
      }
      
      logDest->LogInfo("Loading got %s", gotName.c_str());
      std::shared_ptr<puzzler::Puzzle::Output> got;
      {
         puzzler::FileInStream src(gotName);
         puzzler::PersistContext ctxt(&src, false);

         got=puzzler::PuzzleRegistrar().LoadOutput(ctxt);
      }
      
      if(!ref->Equals(got.get())){
         logDest->LogFatal("Outputs are different.");
         exit(1);
      }
      logDest->LogInfo("Outputs are equal.");
   }catch(std::string &msg){
      std::cerr<<"Caught error string : "<<msg<<std::endl;
      return 1;
   }catch(std::exception &e){
      std::cerr<<"Caught exception : "<<e.what()<<std::endl;
      return 1;
   }catch(...){
      std::cerr<<"Caught unknown exception."<<std::endl;
      return 1;
   }

   return 0;
}

