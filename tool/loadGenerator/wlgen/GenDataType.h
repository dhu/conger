#ifndef GenDataType_H
#define GenDataType_H

#include <iostream>         // for ofstream
#include <common.h>

#include "Types.h"          // for datatype_t

BOREALIS_NAMESPACE_BEGIN

/**Generator Data Type
*/
class GenDataType
{
public:
	/**Return the size of data with specified type
	* @param type the data type
	* @return the size of the data 
	*/
	static int Size(const datatype_t& type);

	/**Typecast a double to the specified type
	* @param type the data type
	* @param data the content
	* @param result
	*/
	static void TypeCast(const datatype_t& type, double data, char* result);

	/**Print the data according to the specified type
	* @param type the data type
	* @param data the content
	*/
	static void printData(const datatype_t& type, char* data);

	/**Output the data according to the specified type
	* @param type the data type
	* @param data hte content
	* @param ostrm
	*/
	static void outputData(const datatype_t& type, char* data, ostream& ostrm);

	/**Print the type
	* @param type the data type
	*/
	static void printType(const datatype_t& type);
};

BOREALIS_NAMESPACE_END
#endif
