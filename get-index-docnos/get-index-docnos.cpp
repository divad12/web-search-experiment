
#include "indri/QueryEnvironment.hpp"
#include <iostream>
#include <vector>
#include <string>

void usage() 
{
  std::cout << "get-index-docnos <repository>" << std::endl;
}

#define REQUIRE_ARGS(n) { if( argc < n ) { usage(); return -1; } }

int main( int argc, char** argv ) 
{
  try 
  {
    REQUIRE_ARGS(2);
    
    indri::collection::Repository r;
    std::string repName = argv[1];
    r.openRead( repName );

    indri::api::QueryEnvironment environment;
    environment.addIndex( repName );

    std::vector<lemur::api::DOCID_T> documentIDs;
    for( int documentID = 1; documentID <= environment.documentCount(); ++documentID ) 
    {
      documentIDs.push_back(documentID);
    }
    std::vector<std::string> docnos = environment.documentMetadata( documentIDs, "docno" ) ;
    
    std::vector<std::string>::iterator iter_docnos = docnos.begin() ;
    for ( ; iter_docnos != docnos.end() ; ++iter_docnos )
    {
      std::cout << *iter_docnos << std::endl ;
    }

    return 0;
  } 
  catch( lemur::api::Exception& e ) 
  {
    LEMUR_ABORT(e);
  }
}


