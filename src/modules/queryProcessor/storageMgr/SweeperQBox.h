#ifndef SWEEPER_QBOX_H
#define SWEEPER_QBOX_H

#include "QBox.h"
#include <list>
#include <vector>
#include <map>

BOREALIS_NAMESPACE_BEGIN

class SweeperQBox : public QBox
{
 protected:
  void setup_impl() throw (AuroraException);
  void init_impl() throw (AuroraException);
  void run_impl(QBoxInvocation&) throw (AuroraException);

 public:
  
  // given a notification, returns the historical tuple
  uint8  *notify( const void * );
  
  // appends TupleQueue to _output
  void set_output_queue( int, ptr<TupleQueue> );

  // returns the TupleQueue for REQUESTS
  ptr<TupleQueue> request_queue();

  // returns the TupleQueue for HISTORICALS
  ptr<TupleQueue> historical_queue();
  
  // Remove a pending request (called when a request is filled by another sweeper)
  void remove_request(uint8  *);  
    
 private:
  // Because we output to TupleQueues,
  map<int, ptr<TupleQueue> > _output;
    
  // Storage for all requests that come in
  list<uint8  *>   _req_buffer;

  // Storage for historicals
  vector<uint8  *> _his_buffer;

  // Returns the first matching request to the historical param
  list<uint8  *>::iterator match_request(uint8 *);
};

BOREALIS_NAMESPACE_END

#endif //SWEEPER_QBOX_H
