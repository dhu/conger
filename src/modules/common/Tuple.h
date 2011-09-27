#ifndef TUPLE_H
#define TUPLE_H

#include "Timestamp.h"
#include "TupleHeader.h"

BOREALIS_NAMESPACE_BEGIN

/// Wrapper class for Tuples, currently used for minor manipulations.
class Tuple
{
  private:
    uint8   *_tuple;

    explicit  Tuple(uint8  *tuple_data) : _tuple(tuple_data) {}


  public:

    static Tuple factory(void  *tuple)
    {
        return(Tuple((uint8 *)tuple));
    }


    static Tuple factory(uint8 *tuple)
    {
        return(Tuple(tuple));
    }


    static const Tuple factory(const string  tuple)
    {
        return(Tuple(const_cast<uint8 *>((uint8 *)tuple.c_str())));
    }


    static const Tuple factory(const void  *tuple)
    {
        return(Tuple(const_cast<uint8 *>((const uint8 *)tuple)));
    }


    static const Tuple factory(const uint8  *tuple)
    {
        return(Tuple(const_cast<uint8 *>(tuple)));
    }


    /// Raw data in the tuple, including header.
    const uint8  *get_data() const
    {
        return( _tuple );
    }


    /// Raw data in the tuple, including header.
    uint8  *get_data()
    {
        return( _tuple );
    }


    Timestamp  get_timestamp() const
    {
        return((Timestamp)(*(TupleHeader *)_tuple).tv);
    }


    Timestamp  get_local_timestamp() const
    {
        return((Timestamp)(*(TupleHeader *)_tuple).local_tv);
    }


    int32  get_tuple_id() const
    {
        return(((TupleHeader *)_tuple)->tuple_id);
    }


    TupleType  get_tuple_type() const
    {
        return(((TupleHeader *)_tuple)->tuple_type);
    }


    int32  get_revision_id() const
    {
        return(((TupleHeader *)_tuple)->rev_id);
    }


    double  get_quality() const
    {
        return(((TupleHeader *)_tuple)->quality);
    }


    int32  get_win_spec() const
    {
        return(((TupleHeader *)_tuple)->win_spec);
    }


    bool  get_fake_flag() const
    {
        if (((TupleHeader *)_tuple)->tuple_flag & FAKE_FLAG)
        {   return(true);
        }

        return(false);
    }


    bool  get_array_flag() const
    {
        if (((TupleHeader *)_tuple)->tuple_flag & ARRAY_FLAG)
        {   return(true);
        }

        return(false);
    }


    int32  get_revised() const
    {
        return(((TupleHeader *)_tuple)->rev_field);
    }


    Timestamp  get_tuple_stime() const
    {
        return((Timestamp)((TupleHeader *)_tuple)->tuple_stime);
    }


    /// The data of the tuple starting after the header.
    const uint8  *get_body() const
    {
        return(_tuple + get_tuple_header_size());
    }


    /// The data of the tuple starting after the header.
    uint8  *get_body()
    {
        return(_tuple + get_tuple_header_size());
    }


    void  set_timestamp(Timestamp  t)
    {
        ((TupleHeader *)_tuple)->tv = (timeval)t;
    }


    void  set_local_timestamp(Timestamp  t)
    {
        ((TupleHeader *)_tuple)->local_tv = (timeval)t;
    }


    void  set_tuple_id(uint32  tid)
    {
        ((TupleHeader *)_tuple)->tuple_id = tid;
    }


    void  set_tuple_type(TupleType  t)
    {
        ((TupleHeader *)_tuple)->tuple_type = t;
    }


    void  set_revision_id(int32  rev_id)
    {
        ((TupleHeader *)_tuple)->rev_id = rev_id;
    }


    void  set_quality(double  q)
    {
        ((TupleHeader *)_tuple)->quality = q;
    }


    void  set_win_spec(int32  spec)
    {
        ((TupleHeader *)_tuple)->win_spec = spec;
    }


    void  set_fake_flag(bool  flag)
    {
        if (flag)
        {   ((TupleHeader *)_tuple)->tuple_flag |= FAKE_FLAG;
        }
        else
        {   ((TupleHeader *)_tuple)->tuple_flag &= !(uint8)FAKE_FLAG;
        }
    }


    void  set_array_flag(bool  flag)
    {
        if (flag)
        {   ((TupleHeader *)_tuple)->tuple_flag |= ARRAY_FLAG;
        }
        else
        {   ((TupleHeader *)_tuple)->tuple_flag &= !(uint8)ARRAY_FLAG;
        }

        return;
    }


    void  set_revised(const int32  r)
    {
        ((TupleHeader *)_tuple)->rev_field = r;
    }


    void set_tuple_stime(Timestamp  t)
    {
        ((TupleHeader *)_tuple)->tuple_stime = (timeval)t;
    }


    void copy_header(const Tuple &tup)
    {
        this->set_timestamp(tup.get_timestamp());
        this->set_local_timestamp(tup.get_local_timestamp());
        this->set_fake_flag(tup.get_fake_flag());
        this->set_array_flag(tup.get_array_flag());
        this->set_tuple_type(tup.get_tuple_type());

        if (get_tuple_header_size() >= 33)
        {
            this->set_win_spec(tup.get_win_spec());
            this->set_quality(tup.get_quality());

            if (get_tuple_header_size() >= 45)
            {
                this->set_revision_id(tup.get_revision_id());
                this->set_tuple_id(tup.get_tuple_id());
                this->set_revised(tup.get_revised());

                if (get_tuple_header_size() == 53)
                {
                    this->set_tuple_stime(tup.get_tuple_stime());
                }
            }
        }
    }


    /// Copy tup onto this assuming tuple size tuple_size.
    ///
    void copy_tuple(const Tuple &tup, int tuple_size)
    {
        memcpy(_tuple, tup._tuple, tuple_size);
    }


    /// When creating a tuple inside a buffer (with Tuple::factory),
    /// one might want to set a default header.
    ///
    void  set_default_header()
    {
        TupleHeader default_header;

        memcpy(_tuple, (void*)&default_header, get_tuple_header_size());
    }


    /// Reset all fields of this tuple to zero assuming size tuple_size.
    ///
    void  clear_tuple(int tuple_size)
    {
        memset(_tuple, 0, tuple_size);
    }


    string  as_string() const;
};


BOREALIS_NAMESPACE_END

#endif                          // TUPLE_H
