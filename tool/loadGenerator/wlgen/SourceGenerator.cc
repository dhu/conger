#include "SourceGenerator.h"

// C++ headers
#include <iostream>         // for ERROR

// Local headers
#include "AttributeSource.h"        // for AttributeSource

BOREALIS_NAMESPACE_BEGIN

void SourceGenerator::initialize(SourcePool* source_pool)
{
	_source_pool = source_pool;
}


bool SourceGenerator::newSources(AggStreamSource    &agg_source)
{

	bool     bad_parameter = false;
	sources_t    new_sources;

	if (agg_source.num_streams < 1)
	{
		ERROR << "Number of streams must be larger than zero." ;
		ERROR << "Generating aggregate stream sources failed. ";
		return false;
	}


	vector<RandomNumGenerator> rate_par_generators;
	for (vector<distribution_t>::iterator
		it = agg_source.rate_parameters.begin();
		it != agg_source.rate_parameters.end();
	++it)
	{
		if (RandomNumGenerator::checkDistribution(*it) == true)
			rate_par_generators.push_back(RandomNumGenerator(*it));
		else
		{
			bad_parameter = true;
			ERROR << "Invalid distribution parameters: ";
			ERROR << "Generating RandomNumGenerators for rate distribution parameters failed.";
			break;
		}
	}

	// generate a list of RandomNumGenerators for each attribute
	// These RandomNumberGenerators will be used to generate the
	// parameters for the attribute distribution
	vector<rand_num_generator_vect_t> source_par_generator_list;

	for (vector<AttributeDef>::iterator
		a_it = agg_source.attributes.begin();
		a_it != agg_source.attributes.end();
	++a_it)
	{
		rand_num_generator_vect_t rand_num_generators;

		for (vector<distribution_t>::iterator
			d_it = (*a_it).source_parameters.begin();
			d_it != (*a_it).source_parameters.end();
		++d_it)
		{
			if (RandomNumGenerator::checkDistribution(*d_it) == false)
			{
				ERROR << "Invalid distribution parameters for source_parameters ";
				ERROR << "Generating RandomNumGenerators for attribue source failed.";
				return false;
			}
			rand_num_generators.push_back(RandomNumGenerator(*d_it));
		}

		source_par_generator_list.push_back(rand_num_generators);
	}

	for (int i=0; i < agg_source.num_streams; ++i)
	{
		// generate stream id
		source_stream_id_t id =
			StreamIDGenerator::getID(agg_source.id.numberID, i);

		// set TimeStampGenerator
		// check type
		if (agg_source.rate_type < 0 ||
			agg_source.rate_type == TIMESTAMP ||
			agg_source.rate_type >= OTHER)
		{
			ERROR << "Invalid distribution type for rate: "
				<< (int)agg_source.rate_type;
			bad_parameter = true;
			break;
		}

		if (agg_source.rate_type == INPUTFILE)
		{
			// check whehter the first attribute is FILENAME type
			if ((rate_par_generators.front()).type() != FILENAME)
			{
				WARN << "The fist parameter of INPUTFILE distribution must be specified by FILENAME distributon.";
				bad_parameter = true;
				break;
			}
		}

		parlist_t rate_pars;
		for (vector<RandomNumGenerator>::iterator it = rate_par_generators.begin();
			it != rate_par_generators.end();
			++it)
		{
			rate_pars.push_back((*it).getRandNum());
		}
		distribution_t tm_generator_distr(agg_source.rate_type, rate_pars);

		if (RandomNumGenerator::checkDistribution(tm_generator_distr) == false)
		{
			ERROR << "Invalid distribution parameters for rate distribution.";
			ERROR << "The the parameters are gengerated from the RandomNumGenerator for rate distribution parameters.";
			bad_parameter = true;
			break;
		}

		// set single_sources
		single_sources_t single_sources;

		vector<rand_num_generator_vect_t>::iterator r_it =
			source_par_generator_list.begin();

		for (vector<AttributeDef>::iterator it = agg_source.attributes.begin();
			it != agg_source.attributes.end();
			++it)
		{
			bool result = newSingleSource(*it, *r_it, single_sources);
			if (result == false)
			{
				ERROR << "Generating attribute source failed. ";

				bad_parameter = true;
				break;
			}
			++r_it;
		}

		if (bad_parameter == true)
			break;

		// new SingleSource
		new_sources.push_back( new SingleSource(
			id,
			agg_source.id,
			tm_generator_distr,
			single_sources,
			agg_source.num_tuples));
	}



	if (bad_parameter == true)
	{
		ERROR << "Generating aggregate stream sources failed. ";
		return false;
	}
	else
	{
		_source_pool->addSources(new_sources);
		NOTICE << "Aggregate stream source generated successfully!";
		return true;
	}
}

bool SourceGenerator::newSingleSource(
									  AttributeDef&         attrdef,
									  vector<RandomNumGenerator>&   randnum_generators,
									  single_sources_t&         single_sources)
{
	if (attrdef.attribute_data_type.type > STRING ||
		attrdef.attribute_data_type.type < 0)
	{
		ERROR << "Wrong data type "
			<< (int)attrdef.attribute_data_type.type ;
		return false;
	}

	if (attrdef.source_type == INPUTFILE)
	{
		// check whehter the first attribute is FILENAME type
		if ((randnum_generators.front()).type() != FILENAME)
		{
			WARN << "The fist parameter of INPUTFILE distribution must be specified by FILENAME distributon.";
			return false;
		}
	}

	parlist_t    parameters;
	for (vector<RandomNumGenerator>::iterator
		it = randnum_generators.begin();
		it != randnum_generators.end();
	++it)
	{
		parameters.push_back((*it).getRandNum());
	}

	distribution_t source_distr(attrdef.source_type, parameters);
	if (RandomNumGenerator::checkDistribution(source_distr) == true)
	{
		single_sources.push_back(
			AttributeSource(
			source_distr,
			attrdef.attribute_data_type));
		return true;
	}
	else
	{
		ERROR << "Invalid generated distribution parameters: attribute distribution.";
		return false;
	}
}

bool SourceGenerator::changeSourceRate(
									   AggStreamSource& agg_source)
{
	bool     bad_parameter = false;

	vector<RandomNumGenerator> rate_par_generators;
	for (vector<distribution_t>::iterator
		it = agg_source.rate_parameters.begin();
		it != agg_source.rate_parameters.end();
	++it)
	{
		if (RandomNumGenerator::checkDistribution(*it) == true)
			rate_par_generators.push_back(RandomNumGenerator(*it));
		else
		{
			bad_parameter = true;
			ERROR << "Invalid distribution parameters: ";
			ERROR << "Generating RandomNumGenerators for rate distribution parameters failed.";
			break;
		}
	}

	if (bad_parameter == true)
	{
		return false;
	}

	sources_t matching_sources;
	_source_pool->lock();
	//get all steams
	_source_pool->getSources(agg_source.id, matching_sources);

	// change rate for each of the SingleSource
	for (sources_iter_t s_it = matching_sources.begin();
		s_it != matching_sources.end();
		++s_it)
	{

		// get TimeStampGenerator distribution
		parlist_t rate_pars;
		for (vector<RandomNumGenerator>::iterator it = rate_par_generators.begin();
			it != rate_par_generators.end();
			++it)
		{
			rate_pars.push_back((*it).getRandNum());
		}
		distribution_t tm_generator_distr(agg_source.rate_type, rate_pars);

		if (RandomNumGenerator::checkDistribution(tm_generator_distr) == false)
		{
			ERROR << "Invalid distribution parameters for rate distribution.";
			ERROR << "The the parameters are gengerated from the RandomNumGenerator for rate distribution parameters.";
			bad_parameter = true;
			break;
		}

		(*s_it)->changeTimestampGenerator(tm_generator_distr);
	}

	_source_pool->unlock();

	if (bad_parameter == true)
		WARN << "Changing rate for stream " << agg_source.id.stringID
		<< "(" << agg_source.id.numberID <<")"
		<< "failed." ;
	else
		WARN << "Changing rate for stream " << agg_source.id.stringID
		<< "(" << agg_source.id.numberID <<")"
		<< "finished." ;

	return (!bad_parameter);

}
BOREALIS_NAMESPACE_END
