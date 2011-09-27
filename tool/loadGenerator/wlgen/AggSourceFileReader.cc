#include "AggSourceFileReader.h"

// C++ headers
#include <iostream>         // for cout, ERROR
#include <sstream>          // for istringstream
#include <cstring>          // for strcpy

// Local headers
#include "AggStreamSource.h"        // for AggStreamSource
#include "Types.h"                  // for distributiontype_t,
#include "RandomNumGenerator.h"     // for RandomNumGenerator*
#include "XmlHandler.h"             // for XmlHandler    

BOREALIS_NAMESPACE_BEGIN

bool AggSourceFileReader::readFile(
								   const char                *filename,
								   vector<AggStreamSource>   &agg_sources,
								   bool                      show_process)
{

	//show_process = false;

	fstream file;
	file.open(filename, ios::in);
	if (! file)
	{
		ERROR << "Can not open file: "<< filename;
		return false;
	}


	XmlHandler xmlhandler;

	//Change XML file to XML Document, get root Node
	DOMDocument *doc;
	doc = xmlhandler.readXmlFileToDoc(filename);
	DOMNode *root;
	if(doc)
	{
		root = (DOMNode*) doc->getDocumentElement();
	}
	else
	{
		ERROR<<"Changing XML file " << filename <<" to XML Document failed.";
	}

	//get vector of input stream name
	vector <char*> input_stream_names;
	input_stream_names = xmlhandler.getInputStreamNames(root);

	//show successfully get stream names
	if(show_process)
	{
		int i = 0;
		for(vector<char*>::iterator it = input_stream_names.begin();
			it !=input_stream_names.end();
			++it)
		{
			DEBUG <<"input stream name "<< i <<": "<<(char*)(*it);
			i++;
		}
	}


	for(vector<char*>::iterator it = input_stream_names.begin();
		it!=input_stream_names.end();
		++it)
	{
		char *stream_name = (char*)(*it);

		if(show_process)
		{
			cout << "Now cope with stream: '"<<stream_name<<"'.";
		}


		AggStreamSource agg_source;

		confAggStreamSource(file, xmlhandler, agg_source, root, stream_name, show_process);

		agg_sources.push_back(agg_source);

	}



	NOTICE << "Reading file \"" << filename << "\" finished.";
	file.close();
	return true;

}

bool AggSourceFileReader::confAggStreamSource(
	fstream             &file,             
	XmlHandler          xmlhandler,
	AggStreamSource     &agg_source,
	const DOMNode       *root,
	const char          *stream_name,
	bool                show_process)
{


	//read number ID
	char *number_id = xmlhandler.getGenInfoAttrValue(root,stream_name,"stream_id");
	if(!strcmp(number_id,""))
	{
		ERROR << "Reading Aggregate stream source number ID failed";
		ERROR << "Data read in: ";

		agg_source.print();
		file.close();
		return false;
	}
	agg_source.id.numberID = atol(number_id);
	if(show_process)
		DEBUG <<"Number ID: " << agg_source.id.numberID;


	//read string ID
	int index = 0;
	//copy stream_name to agg_source.id.stringID
	while(*(stream_name+index)!='\0')
	{
		agg_source.id.stringID[index] = *(stream_name+index);
		index = index + 1;
	}
	*(agg_source.id.stringID + index) = '\0';
	if (show_process)
		DEBUG << "String ID: " << agg_source.id.stringID;


	//read number of streams
	char *stream_number = xmlhandler.getGenInfoAttrValue(root,stream_name,"stream_num");
	if(!strcmp(stream_number,""))
	{
		ERROR<< "Reading number of streams failed";
		ERROR<< "Data read in";
		agg_source.print();
		file.close();
		return false;
	}
	agg_source.num_streams = atol(stream_number);
	if(show_process)
		DEBUG<<"Number of streams: " << agg_source.num_streams;


	//read number of tuples
	char *tuple_number = xmlhandler.getGenInfoAttrValue(root,stream_name,"tuple_num");
	if(!strcmp(stream_number,""))
	{
		ERROR << "Reading number of tuples per stream failed";
		ERROR << "Data read in: ";
		agg_source.print();
		file.close();
		return false;
	}
	agg_source.num_tuples = atol(tuple_number);
	if(show_process)
		DEBUG<<"Number of tuples: " << agg_source.num_tuples;


	//read distribution for this input stream
	if(!AggSourceFileReader::readInputDistribution(file, xmlhandler, agg_source, root, stream_name, show_process))
	{
		ERROR << "Reading input distribution failed." ;
	}


	//read Attribute for this input stream
	if(!AggSourceFileReader::readAttributes(file, xmlhandler, agg_source, root, stream_name, show_process))
	{
		ERROR << "Reading attribute failed.";
	}


	return true;
}


//read input distribution 
bool AggSourceFileReader::readInputDistribution(
	fstream                 &file, 
	XmlHandler              xmlhandler, 
	AggStreamSource         &agg_source, 
	const DOMNode           *root, 
	const char              *stream_name, 
	bool                    show_process)
{

	//read input distribution type    
	int input_dist_type = xmlhandler.getInputDistTypeInt(root, stream_name);

	if (input_dist_type > MAX_DISTRIBUTION_TYPE_NUMBER || input_dist_type < 0)
	{
		ERROR << "Invalid input distribution type: "
			<< input_dist_type;
		ERROR << "Reading input distribution type failed.";
		file.close();
		return false;
	}
	agg_source.rate_type = static_cast<distributiontype_t>(input_dist_type);

	/*
	if(show_process)
	DEBUG<<"Input distribution type: " << agg_source.rate_type;
	*/

	//each parameter of this distribution is a distribution. I call this kind of
	//parameter: "paramDistribution",or "paramDist"

	//read paramDist number of input distribution 
	int temp_params_num = 0;
	int &params_num = temp_params_num;

	if(!xmlhandler.getParamsNum(input_dist_type, params_num))
	{
		ERROR << "error in get parameter number by specific distribuiton";
		file.close();
		return false;
	}


	//params_num is the number of paramDists of this distribution type
	if (params_num>MAX_PARAMETER_NUMBER)
	{
		ERROR << "Invalid number of paramDistributions: "
			<< params_num;
		ERROR << "Reading number of paramDistributions failed.";
		return false;
	}

	if (show_process)
		DEBUG << "Number of paramDistributions: " << params_num;


	//read every paramDistribution of this input stream

	for(int i = 0; i < params_num; i++)
	{
		distribution_t *distr = new(distribution_t);

		//read paramDistribution
		if(readInputParamDist(file, xmlhandler, root, stream_name, i, *distr, show_process))
		{
			agg_source.rate_parameters.push_back(*distr);
			delete distr;
		}
		else
		{
			delete distr;
			ERROR << "Reading ParamDistribution failed." ;
			ERROR << i << "distribution has been read." ;
			return false;

		}
	}
	return true;

}

bool AggSourceFileReader::readInputParamDist(
	fstream         &file, 
	XmlHandler      xmlhandler, 
	const DOMNode   *root,
	const char      *stream_name,
	int             i, 
	distribution_t  &distr, 
	bool            show_process)
{
	int input_paramDist_type = xmlhandler.getInputParamDistTypeInt(root, stream_name, i);

	if (input_paramDist_type > MAX_DISTRIBUTION_TYPE_NUMBER || input_paramDist_type < 0)
	{
		ERROR << "Invalid paramDistribution type: "
			<< input_paramDist_type;
		ERROR << "Reading paramDistribution type failed.";
		file.close();
		return false;
	}

	distr.first = static_cast<distributiontype_t>(input_paramDist_type); 

	if(show_process)
	{
		DEBUG << "paramDistribution type: "
			<< input_paramDist_type;
	}   


	//read parameter number of this paramDistribution
	int temp_params_num = 0;
	int &params_num = temp_params_num;

	if(!xmlhandler.getParamsNum(input_paramDist_type, params_num))
	{
		ERROR << "error in get parameter number by specific paramDistribution";
		file.close();
		return false;
	}

	//params_num is the number of parameters of this paramDistribution type
	if (params_num>MAX_PARAMETER_NUMBER)
	{
		ERROR << "Invalid number of parameters: "
			<< params_num;
		ERROR << "Reading number of parameters failed.";
		return false;
	}

	if (show_process)
		DEBUG << "Number of parameters: " << params_num;


	//read parameters of this paramDistribution
	for(int j = 0; j< params_num; j++ )
	{
		double param = xmlhandler.getInputParamDistParamValue(root, stream_name, i, j);
		distr.second.push_back(param);
		if(show_process)
			DEBUG << "Parameter: " << param;
	}

	//check whether the distribution is valid
	if(RandomNumGenerator::checkDistribution(distr) == true)
		return true;
	else
	{
		ERROR << "Invalid Distribution" ;
		return false;
	}

}

bool AggSourceFileReader::readAttributes(
	fstream                 &file, 
	XmlHandler              xmlhandler, 
	AggStreamSource         &agg_source,
	const DOMNode           *root,
	const char              *stream_name,
	bool                    show_process)
{
	//read number of Attributes
	int attr_num = xmlhandler.getAttrNum(root, stream_name);    
	if(attr_num==0)
	{
		ERROR << "Number of Attribute for stream '"
			<< stream_name
			<< "' is 0.";
		agg_source.print();
		file.close();
		return false;

	}
	if (attr_num > MAX_ATTRIBUTE_NUMBER)
	{
		ERROR << "Invalid number of attributes: "
			<< attr_num;
		ERROR << "Reading number of attributes failed.";
		ERROR << "Reading file failed.";
		ERROR << "Data read in: ";
		agg_source.print();
		file.close();
		return false;
	}

	if (show_process)
		DEBUG << "Number of attributes:" << attr_num;

	for(int i = 0; i< attr_num; i++)
	{
		AttributeDef *attr_def = new(AttributeDef);
		if (readAttributeDef(file, xmlhandler, root, stream_name, i, *attr_def, show_process))
		{
			agg_source.attributes.push_back(*attr_def);
			delete attr_def;
		}
		else
		{
			delete attr_def;
			ERROR << "Reading attributes failed.";
			ERROR << i << " attributes has been read.";
			ERROR << "Reading file failed.";

			file.close();
			return false;
		}

	}


	return true;   
}

bool AggSourceFileReader::readAttributeDef(
	fstream             &file,
	XmlHandler          xmlhandler,
	const DOMNode       *root,
	const char          *stream_name,
	int                 i,
	AttributeDef        &attr_def,
	bool                show_process)
{
	//read attribute data type
	int data_type = xmlhandler.getAttrDataType(root, stream_name, i);

	attr_def.attribute_data_type.type = static_cast<attr_type_t>(data_type);
	if(show_process)
		DEBUG << "Attribute data type: " << data_type;

	int data_length = xmlhandler.getAttrDataLength(root, stream_name, i);

	if (data_length < 0)
	{
		ERROR << "Invalid attribute data length: "
			<< data_length;
		ERROR << "Reading attribute data length failed.";
		return false;
	}

	attr_def.attribute_data_type.length = data_length;
	if (show_process)
		DEBUG << "Attribute data length " << data_length;


	//read distribution
	int attr_dist_type = xmlhandler.getAttrDistTypeInt(root, stream_name, i);
	if (attr_dist_type > MAX_DISTRIBUTION_TYPE_NUMBER || attr_dist_type < 0)
	{
		ERROR << "Invalid distribution type: "
			<< attr_dist_type;
		ERROR << "Reading attribute distribution type failed.";
	}
	attr_def.source_type = static_cast<distributiontype_t>(attr_dist_type);

	if (show_process)
		cout << "Attribute distribution type: " << attr_dist_type;


	//each parameter of this distribution is a distribution. I call this kind of
	//parameter: "paramDistribution",or "paramDist"

	// read paramDist number of Attribute distribution
	int temp_params_num = 0;
	int &params_num = temp_params_num;

	if(!xmlhandler.getParamsNum(attr_dist_type, params_num))
	{
		ERROR << "error in get parameter number by specific distribuiton";
		file.close();
		return false;
	}


	//params_num is the number of paramDists of this distribution type
	if (params_num>MAX_PARAMETER_NUMBER)
	{
		ERROR << "Invalid number of paramDistributions: "
			<< params_num;
		ERROR << "Reading number of paramDistributions failed.";
		return false;
	}

	if (show_process)
		DEBUG << "Number of paramDistributions: " << params_num;


	//read every paramDistribution of this attribute

	for(int j = 0; j < params_num; j++)
	{
		distribution_t *distr = new(distribution_t);

		//read paramDistribution
		if(readAttrParamDist(file, xmlhandler, root, stream_name, i, j, *distr, show_process))
		{
			attr_def.source_parameters.push_back(*distr);
			delete distr;
		}
		else
		{
			delete distr;
			ERROR << "Reading ParamDistribution failed." ;
			ERROR << j << "distribution has been read." ;
			return false;

		}
	}


	return true;
}



bool AggSourceFileReader::readAttrParamDist(
	fstream             &file,
	XmlHandler          xmlhandler,
	const DOMNode       *root,
	const char          *stream_name,
	int                 attr_i,
	int                 param_j,
	distribution_t      &distr,
	bool                show_process)
{
	int attr_paramDist_type = xmlhandler.getAttrParamDistTypeInt(root, stream_name, attr_i, param_j);

	if (attr_paramDist_type > MAX_DISTRIBUTION_TYPE_NUMBER || attr_paramDist_type < 0)
	{
		ERROR << "Invalid paramDistribution type: "
			<< attr_paramDist_type;
		ERROR << "Reading paramDistribution type failed.";
		file.close();
		return false;
	}

	distr.first = static_cast<distributiontype_t>(attr_paramDist_type); 

	if(show_process)
	{
		DEBUG << "paramDistribution type: "
			<< attr_paramDist_type;
	}   

	//read parameter number of this paramDistribution
	int temp_params_num = 0;
	int &params_num = temp_params_num;

	if(!xmlhandler.getParamsNum(attr_paramDist_type, params_num))
	{
		ERROR << "error in get parameter number by specific paramDistribution";
		file.close();
		return false;
	}

	//params_num is the number of parameters of this paramDistribution type
	if (params_num>MAX_PARAMETER_NUMBER)
	{
		ERROR << "Invalid number of parameters: "
			<< params_num;
		ERROR << "Reading number of parameters failed.";
		return false;
	}

	if (show_process)
		DEBUG << "Number of parameters: " << params_num;


	//read parameters of this paramDistribution
	for(int k = 0; k< params_num; k++ )
	{
		double param = xmlhandler.getAttrParamDistParamValue(root, stream_name, attr_i, param_j, k);
		distr.second.push_back(param);
		if(show_process)
			DEBUG << "Parameter: " << param;
	}

	//check whether the distribution is valid
	if(RandomNumGenerator::checkDistribution(distr) == true)
		return true;
	else
	{
		ERROR << "Invalid Distribution" ;
		return false;
	}

	return true;
}

BOREALIS_NAMESPACE_END
