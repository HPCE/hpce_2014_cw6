
#include "puzzler/puzzler.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
   puzzler::PuzzleRegistrar::UserRegisterPuzzles();

   if(argc<2){
      fprintf(stderr, "execute_puzzle isReference logLevel\n");
      exit(1);
   }

   try{
      int isReference=atoi(argv[1]);
      int logLevel=atoi(argv[2]);

      std::shared_ptr<puzzler::ILog> logDest=std::make_shared<puzzler::LogDest>("execute_puzzle", logLevel);
      logDest->Log(puzzler::Log_Info, "Created log.");

      std::shared_ptr<puzzler::Puzzle::Input> input;
      {
         puzzler::StdinStream src;
         puzzler::PersistContext ctxt(&src, false);

         input=puzzler::PuzzleRegistrar().LoadInput(ctxt);
      }

      logDest->Log(puzzler::Log_Info, "Loaded input, puzzle=%s", input->PuzzleName().c_str());

      auto puzzle=puzzler::PuzzleRegistrar().Lookup(input->PuzzleName());

      auto output=puzzle->MakeEmptyOutput(input.get());

      if(isReference){
         logDest->LogInfo("Begin reference");
         puzzle->ReferenceExecute(logDest.get(), input.get(), output.get());
         logDest->LogInfo("Finished reference");
      }else{
         logDest->LogInfo("Begin execution");
         puzzle->Execute(logDest.get(), input.get(), output.get());
         logDest->LogInfo("Finished execution");
      }

      {
         puzzler::StdoutStream dst;
         puzzler::PersistContext ctxt(&dst, true);

         output->Persist(ctxt);
      }

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

