#include "GenDataType.h"

// C++ headers
#include <iostream>         // for cout
#include <iomanip>          // for setw
#include <cmath>            // for floor

BOREALIS_NAMESPACE_BEGIN

int GenDataType::Size(const datatype_t& type)
{
	switch(type.type)
	{
	case INTEGER:
		return sizeof(int);
	case FLOAT:
		return sizeof(float);
	case DOUBLE:
		return sizeof(double);
	case STRING:
		return type.length;
	default:
		return -1;
	}
}

void GenDataType::TypeCast(const datatype_t& type, double data, char* result)
{
	switch(type.type)
	{
	case INTEGER:
		{
			int tmp = static_cast<int>(floor(data + 0.5));
			memcpy(result, reinterpret_cast<char*>(&tmp), sizeof(int));
			return;
		}
	case FLOAT:
		{
			float tmp = static_cast<float>(data);
			memcpy(result, reinterpret_cast<char*>(&tmp), sizeof(float));
			return;
		}
	case DOUBLE:
		{
			double tmp = data;
			memcpy(result, reinterpret_cast<char*>(&tmp), sizeof(double));
			return;
		}
	case STRING:
		{
			data = data - floor(data + 0.0001);
			for (int i = 0; i < type.length; ++i)
			{
				data = data * 100.0;
				int index = (int)(floor(data + 0.0001));
				data = data - index;
				index += 64;
				if (index > 126)
					index -= 64;
				*(result + i) = index;
			}
		}
	default:
		return;
	}
}

void GenDataType::outputData(const datatype_t& type, char* data, ostream& ostrm)
{
	switch(type.type)
	{
	case INTEGER:
		{
			ostrm << setw(10) << *((int*)data);
			ostrm << "   ";
			return;
		}
	case FLOAT:
		{
			ostrm << setw(10) << *((float*)data);
			ostrm << "   ";
			return;
		}
	case DOUBLE:
		{
			ostrm << setw(10) << *((double*)data);
			ostrm << "   ";
			return;
		}
	case STRING:
		{
			for (int i = 0; i < type.length; ++i)
			{
				ostrm << *((char*)(data)+i);
			}
			ostrm << "   ";
			return;
		}
	default:
		return;
	}
}

void GenDataType::printData(const datatype_t& type, char* data)
{
	switch(type.type)
	{
	case INTEGER:
		{
			cout << setw(10) << *((int*)data);
			cout << "   ";
			return;
		}
	case FLOAT:
		{
			cout << setw(10) << *((float*)data);
			cout << "   ";
			return;
		}
	case DOUBLE:
		{
			cout << setw(10) << *((double*)data);
			cout << "   ";
			return;
		}
	case STRING:
		{
			for (int i = 0; i < type.length; ++i)
			{
				cout << *((char*)(data)+i);
			}
			cout << "   ";
			return;
		}
	default:
		return;
	}
}

void GenDataType::printType(const datatype_t& type)
{
	switch(type.type)
	{
	case INTEGER:
		{
			cout << "INTEGER";
			return;
		}
	case FLOAT:
		{
			cout << "FLOAT";
			return;
		}
	case DOUBLE:
		{
			cout << "DOUBLE";
			return;
		}
	case STRING:
		{
			cout << "STRING";
			cout << "(" << type.length << ")";
			return;
		}
	default:
		return;
	}
}

BOREALIS_NAMESPACE_END
