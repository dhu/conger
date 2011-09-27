#ifndef TUPLE_HEADER_H
#define TUPLE_HEADER_H

#include "common.h"

BOREALIS_NAMESPACE_BEGIN


/// The size of the tuple header changes depending on which optional
/// components are in use.  By default all optional fields are included.
///
extern   uint8   HEADER_SIZE;



/// Establish the size of the tuple header.  The size can be reduced once
/// before any streams are processed by the application by sending a
/// component XML element to the QueryProcessor via the Head.
/// 
void  reduce_tuple_header_size(uint8  size);



/// Get the size of the tuple header.  This can only be accessed after the
/// QueryProcessor has had an opportunity to reduce the header size.
///
uint8  get_tuple_header_size();



/**
 * Wrapping in a struct to force usage of the kind: TupleType::INSERTION rather
 * INSERTION alone.
 */
struct TupleType
{

  public:

    typedef enum
    {
        // Standard types
        INSERTION = 0,
        DELETION,
        REPLACEMENT,
        HISTORICAL,

        // Magda's types used for fault-tolerance
        TENTATIVE,  // Tentative version of INSERTION (tentative deletions/replacements not supported)
        BOUNDARY,   // Tuple acting both as punctuation and hearbeat
        UNDO,       // Tuple indicating that state should be rolled back to undo effects of previous tuples 
        UP_FAILURE, // SUnion signals that it enters failure state
        RECONCILIATION_REQUEST, // SUnion signals that it can now reconcile its state
        RECONCILIATION_DONE,    // SUnion signals that it reconciled its state and caught up with input
        UNDO_REDO_START,        // ConsistencyMngr requests that SUnions reconciles by propagating an UNDO
        // End of Magda's types used for fault-tolerance

        DELIMITER,    //for revisions
        REQUEST,
        NOTIFICATION,
        
        // Default
        UNKNOWN

    } Type;


  private:

    Type _type;


  public:

    TupleType(Type t = UNKNOWN) : _type(t) {}


    bool operator==(const TupleType& t) const
    {
        return (t._type == _type);
    }


    bool operator!=(const TupleType& t) const
    {
        return (t._type != _type);
    }


    /// Method that allows DEBUG << tuple_type;
    string as_string() const
    {
        // To avoid a long switch statement, using array of strings
        // (keeping the strings here so it's easier to remember to modify this array when adding new types)
        string type_strings[] = { "INSERTION", "DELETION", "REPLACEMENT", "HISTORICAL",
                                  "TENTATIVE", "BOUNDARY", "UNDO", "UP_FAILURE",
                                  "RECONCILIATION_REQUEST", "RECONCILIATION_DONE", "UNDO_REDO_START", 
                                  "DELIMITER", "REQUEST", "NOTIFICATION" };

        if ( _type <= NOTIFICATION )
        {   return type_strings[_type];
        }

        return string("UNKNOWN");
    }

    int as_int() const
    {
        return( _type == INSERTION ? 0 :
                _type == DELETION  ? 1 :
                _type == REPLACEMENT ? 2 :
                _type == HISTORICAL ? 3 :
                _type == TENTATIVE ? 4 :
                _type == BOUNDARY ? 5 :
                _type == UNDO ? 6 :
                _type == UP_FAILURE ? 7 :
                _type == RECONCILIATION_REQUEST ? 8 :
                _type == RECONCILIATION_DONE ? 9 :
                _type == UNDO_REDO_START ? 10 :
                _type == DELIMITER ? 11 :
                _type == REQUEST ? 12 :
                _type == NOTIFICATION ? 13 : 14 );
    }

} __attribute__((__packed__));
NMSTL_TO_STRING(TupleType);


/// Set if the tuple is a fake tuple, zero by default.
///
#define FAKE_FLAG          0x08


/// Set when an input tuple is received if the tuple contains any array fields.
///
#define ARRAY_FLAG         0x04



/// Common tuple header fields.
/// This structure is only used by applications.
///
struct TupleBase
{
    /// System time assigned by the query processor at the first Borealis node
    /// that receives this tuple.
    ///
    timeval     tv;          // 0..7 

    /// System time assigned by the query processor at the current Borealis node.
    ///
    timeval     local_tv;    // 8..15

    /// A flag that indicates if the tuple is a fake tuple or has arrays.
    /// By default all flags are zero.
    /// used by: Load Shedder and Array processing.
    ///
    uint8       tuple_flag;  // 16

    /// source-assigned tuple type
    /// possible values are: {INSERTION, DELETION, REPLACEMENT, HISTORICAL}
    /// used by: Revision Processor and Network Partitions
    /// TO DO:  byte
    ///
    TupleType   tuple_type;  // 17

} __attribute__((__packed__));



/// Include extended fields for load shedding.
/// This structure is only used by applications.
///
struct TupleShedder : TupleBase
{
    /// window specification, -1 by default
    /// used by: Load Shedder
    /// TO DO:  uint16  or  uint8
    ///
    int32       win_spec;    // 20..23


    /// source-assigned tuple quality
    /// can represent various metrics like priority, confidence, etc.
    /// used by: Load Shedder
    /// TO DO:  single
    ///
    double      quality;     // 18..19

} __attribute__((__packed__));



/// Include extended fields for revision processing and load shedding.
/// This structure is only used by applications.
///
struct TupleRevision : TupleShedder
{
    /// revision id, -1 by default
    /// used by: Revision Processor
    ///
    int32       rev_id;      // 24..27


    /// source-assigned unique tuple id
    /// used by: Revision Processor and HA
    ///
    int32       tuple_id;    // 28..31


    /// Index to a revised field.
    /// used by: Revision Processor
    /// TO DO:  uint16
    ///
    int32       rev_field;   // 32..33

} __attribute__((__packed__));



///  Full tuple header including all extended fields.
///  This structure is used throught the Borealis code base.
///  Extended fields can only be accessed if the corresponding modules are.
///  enabled.  The enabled fields are in the catalog.
///
struct TupleHeader : public TupleRevision
{
    /// Magda: Timestamp value used for tuple serialization.
    ///
    timeval     tuple_stime; // 34..37


    TupleHeader()
    {
        static int counter = 0; // for system-wide unique input tuple id

        tv.tv_sec        = 0;   // Timestamp set at input to the first node.
        tv.tv_usec       = 0;

        local_tv.tv_sec  = 0;   // Timestamp set at input to each node.
        local_tv.tv_usec = 0;

        tuple_flag       = 0;
        tuple_type       = TupleType::INSERTION;

        if (get_tuple_header_size() >= 33)
        {
            win_spec = -1;
            quality  = 0;

            if (get_tuple_header_size() >= 45)
            {
                rev_id    = -1;
                tuple_id  = ++counter;
                rev_field = -1;

                if (get_tuple_header_size() == 53)
                {
                    tuple_stime.tv_sec  = 0;
                    tuple_stime.tv_usec = 0;
                }
            }
        }
    }


    string as_string() const
    {
        string  out;

        out << "TupleHeader{timestamp=" << tv.tv_sec << "." << tv.tv_usec
            << ",local_timestamp=" << local_tv.tv_sec << "." << local_tv.tv_usec
            << ",tuple_flag=" << tuple_flag
            << ",type=" << tuple_type;

        if (get_tuple_header_size() >= 33)
        {
            out << ",win_spec=" << win_spec;

            if (get_tuple_header_size() >= 45)
            {
                out << ",rev_id=" << rev_id
                        << ",id=" << tuple_id
                 << ",rev_field=" << rev_field;

                if (get_tuple_header_size() == 53)
                {
                     out << ",tuple_stime=" << tuple_stime.tv_sec
                                     << "." << tuple_stime.tv_usec;
                }
            }
        }

        out << "}";

        return out;
    }

} __attribute__((__packed__));

NMSTL_TO_STRING(TupleHeader);

BOREALIS_NAMESPACE_END

#endif                       // TUPLE_HEADER_H
