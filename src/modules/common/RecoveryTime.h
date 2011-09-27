/*
 * @(#)RecoveryTime.h
 */

#ifndef RecoveryTime_H
#define RecoveryTime_H

#include "common.h"
#include "MedusaID.h"

BOREALIS_NAMESPACE_BEGIN

#define TIME double

/**
 * The RecoveryTime class implements RecoveryTimes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 08/20/2003
 */
class RecoveryTime {

  // Attribute(s)
  protected:

    /**
     * The times.
     */
    vector<TIME> _times;

    /**
     * The recovery times.
     */
    vector<TIME> _recovery_times;
    
    /**
     * The recovery times.
     */
    TIME _start_time;

    /**
     * Has ever printed.
     */
    bool _printed;

   // Method(s)
  public:

    /**
     * Constructs a new RecoveryTime.
     */
    RecoveryTime();

    /**
     * Returns a string representation of this RecoveryTime.
     */
    string as_string() const;
    
    void print(ptr<ofstream> o);
    
    /**
     * The set operator.
     */
    void set(RecoveryTime& recovery_time);

    /**
     * Puts a recovery time.
     */
    void put_recovery_time(Time t, Time recovery_time);
    
    /**
     * Adds the specified RecoveryTime to this RecoveryTime.
     */
    void add(RecoveryTime& recovery_time);

    /**
     * Takes the maximum between This RecoveryTime and specified RecoveryTime.
     */
    void max(RecoveryTime& recovery_time);
    
    /**
     * Returns the average of This RecoveryTime.
     */
    TIME get_average();
    
    /**
     * Clears this RecoveryTime.
     */
    void clear();

    TIME get_time_span();
    
    int get_number_of_drops();

  protected:    

    void insert(unsigned int i, TIME x);
    
    void erase(unsigned int i);

    void insert(unsigned int i, TIME x, TIME y);

    void truncate_before(TIME time);

    void align(RecoveryTime& recovery_time);

    NMSTL_SIMPLY_SERIALIZABLE(RecoveryTime, << _times << _recovery_times 
            << _start_time << _printed);

};

NMSTL_TO_STRING(RecoveryTime);

BOREALIS_NAMESPACE_END
#endif


