#ifndef REGEXCONVERTER_BOX_H
#define REGEXCONVERTER_BOX_H

#include "SimpleConverterBox.h"
#include <sys/types.h>
#include <regex.h>

BOREALIS_NAMESPACE_BEGIN


/// Converts records matching a regular expression to tuples on a
/// single stream.
class RegexConverterBox : public SimpleConverterBox
{
  protected:
    /// Creates a regex converter box.
    ///
    /// @param schema the schema of tuples generated by the box
    /// @param delim the terminator of each input record
    /// @param regex the regex which each input record will match;
    ///     subexpressions in the regex will be provided to the
    ///     convertTupleFromRegex() method
    RegexConverterBox( TupleDescription  schema,
                                   char  delim,
                                 string  regex
                     )
                throw( AuroraException );

  public:
    /// Destructor.
    ~RegexConverterBox();

    /// A subexpression matched by a regular expression.  Comprises a
    /// range of bytes in the input received over the network.
    struct Subexpr
    {
        /// Dummy constructor.
        Subexpr() {}

        /// Constructs a subexpression comprising a range of bytes.
        Subexpr(const char *begin, const char *end) : begin(begin), end(end) {}

        /// The beginning of the subexpression.
        const char  *begin;

        /// The end of the subexpression.
        const char  *end;

        /// Returns the subexpression as a string.
        string as_string() const { return string( begin, end ); }

        /// Saves the subexpression as a string field.
        ///
        /// @param buf a string field in a tuple structure; must be
        ///     of type <code>char[]</code>
        /// @return true if the subexpression was completely copied,
        ///     or false if it was truncated
        template <typename char_array>
        bool to_string_field(char_array& buf) const
        {
            return to_string_field( buf, sizeof buf / sizeof buf[0] );
        }

        /// Saves the subexpression as a string field.
        ///
        /// @param field a buffer in which to save the subexpression
        /// @param size the maximum number of characters to save
        /// @return true if the subexpression was completely copied,
        ///     or false if it was truncated
        bool to_string_field( char *field, unsigned int  size ) const;

        /// Converts the subexpression to a 32 bit integer value.
        ///
        /// @throw AuroraException if the subexpression is not a valid
        /// 32 bit integer
        int32 to_int() const throw( AuroraException );

        /// Converts the subexpression to a 64 bit integer value.
        ///
        /// @throw AuroraException if the subexpression is not a valid
        /// 64 bit integer
        int64 to_long() const throw( AuroraException );

        /// Converts the subexpression to a 32 bit floating-point value.
        ///
        /// @throw AuroraException if the subexpression is not a valid
        ///     single precision floating-point value
        single to_single() const throw( AuroraException );

        /// Converts the subexpression to a 64 bit floating-point value.
        ///
        /// @throw AuroraException if the subexpression is not a valid
        ///     double precision floating-point value
        double to_double() const throw( AuroraException );
     };

    /// Handles data by matching input data against the regex and calling
    /// convertTupleFromRegex.   Should not be overridden by subclasses (if
    /// you would need to override this, subclass SimpleConverterBox
    /// directly).
    virtual bool convert_tuple(const char *data, unsigned int length, char *out)
        throw (AuroraException);

    /// Convert an input record that matches the regex provided in the
    /// constructor.  Must be implemented in the subclass.
    ///
    /// @param match    the entire input record matched by the regex
    /// @param subexprs the vector of subexpressions matched by the regex
    /// @param out      a buffer in which to store the converted tuple
    /// @return true if the record was successfully converted
    virtual bool convert_tuple_from_regex(Subexpr match,
                                       const vector<Subexpr>& subexprs,
                                       char *out)
        throw (AuroraException) = 0;

  private:
    string _regex;
    ptr<regex_t> _preg;

    // vector, to avoid constant reallocations
    vector<Subexpr> _subexprs;
};

BOREALIS_NAMESPACE_END
#endif                      // REGEXCONVERTER_BOX_H
