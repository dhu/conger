#include "AggStreamSource.h"

// C++ headers
#include <cstring>      // for strcpy
#include <iostream>     // for cout
#include <iomanip>      // for setw


BOREALIS_NAMESPACE_BEGIN

AggStreamSource::AggStreamSource():
num_streams(0),
num_tuples(0),
rate_type(OTHER)
{
	id.numberID = 0;
	strcpy(id.stringID, "Not Assigned");
}

void AggStreamSource::print()
{
	cout << "-------------- Aggregate Stream Source ----------------"
		<< endl;
	cout << setw(25) << "String ID:"
		<< setw(25) << id.stringID
		<< endl;
	cout << setw(25) << "Number ID:"
		<< setw(25) << id.numberID
		<< endl;
	cout << setw(25) << "Num of Streams:"
		<< setw(25) << num_streams
		<< endl;
	cout << setw(25) << "Num of Tuples:"
		<< setw(25) << num_tuples
		<< endl;
	cout << setw(25) << "Num of Attrs:"
		<< setw(25) << attributes.size()
		<< endl;

	cout << endl;
	cout << setw(25) << "Rate Type:";

	printDistributionType(rate_type, false);
	cout << endl;

	printDistributionVector(rate_parameters);

	int i = 0;
	for (vector<AttributeDef>::iterator it = attributes.begin();
		it != attributes.end();
		++it)
	{
		++i;
		cout << endl;
		cout << setw(10) << "Attribute " << setw(15) << i;
		printAttibueDef(*it);
	}

	cout << "-------------------------------------------------------"
		<< endl;
}

void AggStreamSource::printDistributionVector(
	vector<distribution_t>& distr_v)
{
	int i = 0;
	for (vector<distribution_t>::iterator it = distr_v.begin();
		it != distr_v.end();
		++it)
	{
		++i;
		cout << setw(5) << ""
			<< setw(10) << "Parameter "
			<< setw(10) << i;
		printDistribution(*it);
	}
}

void AggStreamSource::printDistribution(distribution_t& distr)
{
	cout << setw(25);
	printDistributionType(distr.first, false);
	for (parlist_it_t it = distr.second.begin();
		it != distr.second.end();
		++it)
	{
		cout << "("
			<< *it << ")";
	}
	cout << endl;
}

void AggStreamSource::printAttibueDef(AttributeDef& attr_def)
{
	cout << setw(25);
	printDistributionType(attr_def.source_type, true);
	printDateType(attr_def.attribute_data_type.type);
	cout << "("
		<< attr_def.attribute_data_type.length << ")"
		<< endl;
	printDistributionVector(attr_def.source_parameters);
}

void AggStreamSource::printDateType(attr_type_t type)
{
	switch (type)
	{
	case INTEGER:
		cout << "INTEGER";
		break;
	case FLOAT:
		cout << "FLOAT";
		break;
	case DOUBLE:
		cout << "DOUBLE";
		break;
	case STRING:
		cout << "STRING";
		break;
	default:
		cout << "WRONG TYPE: " << (int)type;
	}
}

void AggStreamSource::printDistributionType(
	distributiontype_t type,
	bool is_source)
{
	switch (type)
	{
	case CONSTANT:
		cout << "CONSTANT";
		break;
	case UNIFORM:
		cout << "UNIFORM";
		break;
	case NORMAL:
		cout << "NORMAL";
		break;
	case EXPONENTIAL:
		cout << "EXPONENTIAL";
		break;
	case LINEAR:
		cout << "LINEAR";
		break;
	case RANDOMWALK:
		cout << "RANDOMWALK";
		break;
	case RANDOMDIRECTIONWALK:
		cout << "RANDOM_DIRECTION_WALK";
		break;
	case TIMESTAMP:
		cout << "TIMESTAMP";
		break;
	case INPUTFILE:
		cout << "INPUTFILE";
		break;
	case FILENAME:
		cout << "FILENAME";
		break;
	case OTHER:
		cout << "OTHER";
		break;
	default:
		cout << "WRONG TYPE: " << (int)type;
	}
}

BOREALIS_NAMESPACE_END
