
#include "puzzler/puzzler.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
   puzzler::PuzzleRegistrar::UserRegisterPuzzles();

   if(argc<2){
      fprintf(stderr, "run_puzzle name scale logLevel\n");
      std::cout<<"Puzzles:\n";
      puzzler::PuzzleRegistrar::ListPuzzles();
      exit(1);
   }

   try{
      std::string name=argv[1];

      int scale=atoi(argv[2]);

      // Control how much is being output.
      // Higher numbers give you more info
      int logLevel=atoi(argv[3]);
      fprintf(stderr, "LogLevel = %s -> %d\n", argv[3], logLevel);

      std::shared_ptr<puzzler::ILog> logDest=std::make_shared<puzzler::LogDest>("run_puzzle", logLevel);
      logDest->Log(puzzler::Log_Info, "Created log.");

      auto puzzle=puzzler::PuzzleRegistrar::Lookup(name);
      if(!puzzle)
	 throw std::runtime_error("No puzzle registered with name "+name);

      logDest->LogInfo("Creating random input");
      auto input=puzzle->CreateInput(logDest.get(), scale);

      logDest->LogInfo("Executing puzzle");
      auto got=puzzle->MakeEmptyOutput(input.get());
      puzzle->Execute(logDest.get(), input.get(), got.get());

      logDest->LogInfo("Executing reference");
      auto ref=puzzle->MakeEmptyOutput(input.get());
      puzzle->ReferenceExecute(logDest.get(), input.get(), ref.get());

      logDest->LogInfo("Checking output");
      if(!ref->Equals(got.get())){
         logDest->LogFatal("Output is not correct.");
         exit(1);
      }
      logDest->LogInfo("Output is correct");


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

