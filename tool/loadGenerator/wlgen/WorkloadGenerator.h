#ifndef WorkloadGenerator_H
#define WorkloadGenerator_H

#include <common.h>

// Local headers
#include "SourceGenerator.h"
#include "SourcePool.h"
#include "StreamBuffer.h"
#include "WGController.h"
#include "StreamSender.h"
#include "AggStreamSource.h"
#include "AggSourceFileReader.h"


BOREALIS_NAMESPACE_BEGIN

/** Workload Generator
 */
class WorkloadGenerator
{
public:
   /**Construct a WorkloadGenerator with default parameters
    */
   WorkloadGenerator();

   /**Construct the workload generator at given port
    * @param port port number
    */
   WorkloadGenerator(int port);

   /**Start a command line interface. This interface provide all the functions the user needs.
    */
   void startCommandReceiver();

   //For GUI command receiver server. 
   
   /**Add an AggStreamSource.
    * @param agg_source the new aggregate Stream Source
    */
   void addAggSource(AggStreamSource& agg_source);

   /**Change the rate of all sources
    * @param ratio rate
    */
   void changeRate(double ratio);

   /**Change the rate of a given source
    * @param agg_source the aggregate source that to be changed
    */
   void changeRate(AggStreamSource& agg_source);

   /**Delete all sources
    */
   void emptySourses();

   // For file based command
   
   /**Read a file that constains the configureation for a AggStreamSource
    * @param filename the name of the file
    */
   void readAggInfoFile(char *filename);

   /**Read files that constains the file names of the AggStreamSource configure
    * @param filename the names of the files
    */
   void readAggInfoFiles(char *filename);

   /**Change the rate of all agg sources readed from a file
    * @param filename the name of the file
    */
   void changeRateByFile(char *filename);

   /**Print the AggStreamSource infomaito to a file
    * @param filename the name of the file
    */
   void printAggInfoFile(char *filename);

   /** Print the partial informaiton of a AggStreamSource to a file
    * @param filename the name of the file
    */    
   void printPartialAggInfoFile(char *filename);

   /**Delete a AggStreamSource with given number ID
    * @param numberID the number ID
    */
   void deleteAggSource(int numberID);

   /**Delete a AggStreamSource with given stream ID
    * @param stringID the string ID
    */
   void deleteAggSource(char *stringID);

   /**Sending exist tuples from a file
    *
    */
   void sendExistTuples(char *filename);
   
   
   
   /**Get varible _stream_buffer
    * 
    */
   /*
   StreamBuffer getStreamBuffer()
   {
       return _stream_buffer;
   };

   StreamSender getStreamSender()
   {
        return _stream_sender;
   };
   */
private:
   SourceGenerator      _source_generator;
   SourcePool           _source_pool;
   StreamBuffer         _stream_buffer;
   WGController         _controller;
   StreamSender         _stream_sender;

   void printUsage();
};

BOREALIS_NAMESPACE_END
#endif
