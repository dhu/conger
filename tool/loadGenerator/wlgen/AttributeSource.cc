#include "AttributeSource.h"

BOREALIS_NAMESPACE_BEGIN

AttributeSource::AttributeSource(
								 const distribution_t& source_distr,
								 datatype_t type):
_data_type(type),
_randnumber_generator(source_distr)
{
	_data_size = GenDataType::Size(type);
};

distributiontype_t AttributeSource::getSourceType() const
{
	return _randnumber_generator.type();
}

int AttributeSource::getDataSize() const
{
	return _data_size;
}

datatype_t AttributeSource::getDataType() const
{
	return _data_type;
}


void AttributeSource::initialize(Timeval first_time)
{
	_last_time = first_time;
}

int AttributeSource::getNextData(const Timeval& newtime, char* data)
{
	switch (_randnumber_generator.type())
	{
	case CONSTANT:
	case UNIFORM:
	case NORMAL:
	case EXPONENTIAL:
		GenDataType::TypeCast(
			_data_type,
			_randnumber_generator.getRandNum(),
			data);
		break;
	case LINEAR:
	case RANDOMWALK:
	case RANDOMDIRECTIONWALK:
		{
			Timeval tmp = newtime;
			tmp -= _last_time;
			double interval = tmp.double_value();
			_last_time = newtime;

			double new_value = _randnumber_generator.getRandNum(interval);
			GenDataType::TypeCast(
				_data_type,
				new_value,
				data);
			break;
		}
	case TIMESTAMP: // timestamp data source
		GenDataType::TypeCast(_data_type, newtime.double_value(), data);
		break;
	default:
		GenDataType::TypeCast(
			_data_type,
			_randnumber_generator.getRandNum(),
			data);
	}
	return _data_size;

}
BOREALIS_NAMESPACE_END
