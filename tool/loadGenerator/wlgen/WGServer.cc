#include "WGServer.h"

// C++ headers
#include <map>          // for pair
#include <iostream>     // for cout, ERROR, endl
#include <cstdio>       // for sprintf

// Local headers
#include "Types.h"
#include "AggStreamSource.h"
#include "XmlHandler.h"         // for XmlHandler
#include "TuplesSave.h"

BOREALIS_NAMESPACE_BEGIN


WGServer::WGServer(WorkloadGenerator *workload_generator):
_port(DEFAULT_PORT),
_num_handler_threads(DEFAULT_NUM_HANDLERS),
_server(0),
_workload_generator(workload_generator)
{
}

WGServer::WGServer(
				   WorkloadGenerator *workload_generator,
				   int port,
				   int num_handler_threads):
_port(port),
_num_handler_threads(num_handler_threads),
_server(0),
_workload_generator(workload_generator)
{
}


WGServer::~WGServer()
{
	if (_server != 0)
		delete _server;
}

bool WGServer::initialize()
{
	NOTICE << "Starting GUI command receiver... ";
	_server = new SocketServer(
		_port,
		DEFAULT_TYPE,
		_num_handler_threads,
		(void(*)(int, void*))serverHandler,
		this);
	if (_server->start())
	{
		cout << "input receiver started at port "
			<< _port
			;
		return true;
	}
	else
	{
		return false;
	}
}

bool WGServer::readSocket(int socket, char *buf, int max_len, int &file_length)
{
	int rval = 100;
	int i = 0;
	while ( rval > 0 && i < max_len)
	{
		rval = read(socket, buf + i, 1);
		i = i + 1;

		if (rval == 0)
		{
			cout << "WGServer: connection closed by gui at socket "
				<< socket
				;
			close(socket);
			//return false;
		}
		if (rval == -1)
		{
			cout << "WGServer: read socket " << socket << " error." ;
			perror("");
			close(socket);
			return false;
		}

	}
	file_length = i-1;

	return true;
}


void WGServer::serverHandler(int sock, WGServer* me)
{
	// This function is writen by arsinger to receive command packet from
	// GUI

	cout << endl << "Request received from gui" ;
	vector<AggStreamSource> streams;

	char buf[MAX_BUFFER_SIZE];

	int file_length = 0;
	if(!readSocket(sock,buf,MAX_BUFFER_SIZE, file_length))
	{
		NOTICE << "Reading data from GUI failed.";
	}


	//save buf into a file
	fstream socket_file;
	char *filename = "/tmp/GenSocketTemp";

	socket_file.open(filename, ios::out);
	if(!socket_file)
	{
		ERROR << "File " << filename << "can't open. Can not read command from GUI.";
	}

	for(int i = 0; i< file_length; i++)
	{
		socket_file << *(buf+i);
	}
	socket_file.close();



	//read socket file. Excute command depend on its content.
	socket_file.open(filename, ios::in);

	if(!socket_file)
	{
		ERROR << "File " << filename << "can't open. Can not read command from GUI.";
	}

	char command[5];
	if(readLine(socket_file, command, sizeof(command))==false)
	{
		ERROR << "Reading command from GUI failed.";
	}

	if(!strcmp(command,"stop"))
	{
		NOTICE << "Stop generating streams";
		socket_file.close();

		me->_workload_generator->emptySourses();
		close(sock);

	}
	else if(!strcmp(command,"rate"))
	{
		if(readLine(socket_file, command, sizeof(command))==false)
		{
			ERROR << "Reading command from GUI failed.";
		}

		socket_file.close();

		char *stop_string;
		double ratio = strtod(command,&stop_string);
		NOTICE << "Change aggregate stream rate to: "
			<< ratio;

		for(vector<AggStreamSource>::iterator it = streams.begin();
			it !=streams.end();
			++it)
		{
			AggStreamSource stream = (AggStreamSource)(*it);
			stream.print();
		}

		me->_workload_generator->changeRate(ratio);
		close(sock);

	}
	else if(!strcmp(command, "tupl"))
	{

		char tuple_filename[MAX_BUFFER_SIZE];
		if(readLine(socket_file, tuple_filename, sizeof(tuple_filename))==false)
		{
			ERROR << "Reading command from GUI failed.";
		}

		socket_file.close();

		NOTICE << "Command from GUI: Sending exist tuples from tuple file: " << tuple_filename;

		//sending
		me->_workload_generator->sendExistTuples(tuple_filename);

		close(sock);

	}

	else
	{
		//read socket file, change it to xml document 
		socket_file.close();

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
			ERROR<< "Changing XML file '/tmp/GenSocketTemp' to XML Document failed.";
		}

		//see weather it is a distribution file or a data file
		char *root_elem_name = XMLString::transcode(root->getNodeName());



		if(!strcmp(root_elem_name, "borealis"))
		{

			//It is a distribution file   
			// read in the information for aggegate stream source
			//call readAggStreamInfo. read distribution information.
			if (!readAggStreamInfo(socket_file, xmlhandler, root, streams))
			{
				WARN << "Wrong data packet from GUI" ;

				for(vector<AggStreamSource>::iterator it = streams.begin();
					it !=streams.end();
					++it)
				{            

					AggStreamSource stream = (AggStreamSource)(*it);
					stream.print();
				}

				close(sock);
				return;
			}

			//Adding aggregate stream source:
			NOTICE << "Adding aggregate stream sources: " ;

			for(vector<AggStreamSource>::iterator it = streams.begin();
				it !=streams.end();
				++it)
			{
				AggStreamSource stream = (AggStreamSource)(*it);
				stream.print();
				me->_workload_generator->addAggSource(stream);
			}
			close(sock);

		}

	}
}



bool WGServer::readAggStreamInfo(
								 fstream                 &file,
								 XmlHandler              xmlhandler,
								 const DOMNode           *root,
								 vector<AggStreamSource> &streams)
{

	//temp
	bool show_process = false;
	if(show_process)
		cout << "begin to read '/tmp/GenSocketTemp.xml' File." <<endl;


	//get vector of input stream name
	vector <char*> input_stream_names;
	input_stream_names = xmlhandler.getInputStreamNames(root);


	for(vector<char*>::iterator it = input_stream_names.begin();
		it!=input_stream_names.end();
		++it)
	{
		char *stream_name = (char*)(*it);

		if(show_process)
		{
			cout << "Now cope with stream: '"<<stream_name<<"'."<<endl;
		}

		AggStreamSource stream;

		confAggStreamSource(file, xmlhandler, stream, root, stream_name, show_process);

		streams.push_back(stream);
	}

	return true;

}

bool WGServer::confAggStreamSource(
								   fstream             &file,             
								   XmlHandler          xmlhandler,
								   AggStreamSource     &stream,
								   const DOMNode       *root,
								   const char          *stream_name,
								   bool                show_process)
{


	//read number ID
	char *number_id = xmlhandler.getGenInfoAttrValue(root,stream_name,"stream_id");


	if(!strcmp(number_id,""))
	{
		ERROR << "Reading Aggregate stream source number ID failed" ;
		file.close();
		return false;
	}
	stream.id.numberID = atol(number_id);

	if(show_process)
	{
		cout<<"stream number ID: " << stream.id.numberID <<endl;
	}

	//read string ID
	int index = 0;
	//copy stream_name to stream.id.stringID
	while(*(stream_name+index)!='\0')
	{
		stream.id.stringID[index] = *(stream_name+index);
		index = index + 1;
	}
	*(stream.id.stringID + index) = '\0';
	if (show_process)
		cout << "String ID: " << stream.id.stringID <<endl;



	//read number of streams
	char *stream_number = xmlhandler.getGenInfoAttrValue(root,stream_name,"stream_num");
	if(!strcmp(stream_number,""))
	{
		ERROR << "Reading number of streams failed" ;
		file.close();
		return false;
	}
	stream.num_streams = atol(stream_number);

	if(show_process)
	{
		cout<<"number of streams: " <<stream.num_streams <<endl;
	}

	if (stream.num_streams <= 0)
	{
		cout << "Number of streams must be larger than zero." << endl ;
		return false;
	}

	//read number of tuples
	char *tuple_number = xmlhandler.getGenInfoAttrValue(root,stream_name,"tuple_num");
	if(!strcmp(stream_number,""))
	{
		ERROR << "Reading number of tuples per stream failed" ;

		file.close();
		return false;
	}
	stream.num_tuples = atol(tuple_number);

	if(show_process)
	{
		cout<<"number of tuples: " << stream.num_tuples <<endl;
	}

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
	stream.rate_type = static_cast<distributiontype_t>(input_dist_type);

	/*
	//temp
	if(show_process)
	{
	cout<<"distribution type of the input: " << stream.rate_type <<endl;
	}
	*/

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

		return false;
	}

	//read every paramDistribution of this input stream
	for(int i = 0; i < params_num; i++)
	{
		distribution_t *distr = new(distribution_t);

		//read paramDistribution
		if(readInputParamDist(file, xmlhandler, root, stream_name, i, *distr, show_process))
		{
			stream.rate_parameters.push_back(*distr);
			delete distr;
		}
		else
		{
			delete distr;
			ERROR << "Reading ParamDistribution failed.";
			ERROR << i << "distribution has been read." ;
			return false;

		}
	}

	// Now read attributes
	//read number of Attributes
	int attr_num = xmlhandler.getAttrNum(root, stream_name);    
	if(attr_num==0)
	{
		ERROR << "Number of Attribute for stream '"
			<< stream_name
			<< "' is 0.";
		file.close();
		return false;

	}
	if (attr_num > MAX_ATTRIBUTE_NUMBER)
	{
		ERROR << "Invalid number of attributes: "
			<< attr_num;
		file.close();
		return false;
	}

	for(int i = 0; i< attr_num; i++)
	{
		AttributeDef *attr_def = new(AttributeDef);
		if (readAttributeDef(file, xmlhandler, root, stream_name, i, *attr_def, show_process))
		{
			stream.attributes.push_back(*attr_def);
			delete attr_def;
		}
		else
		{
			delete attr_def;
			ERROR << "Reading attributes failed.";
			ERROR << i << " attributes has been read.";

			file.close();
			return false;
		}

	}

	return true;
}



bool WGServer::readInputParamDist(
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
		file.close();
		return false;
	}

	distr.first = static_cast<distributiontype_t>(input_paramDist_type); 

	/*
	//temp
	if(show_process)
	{
	cout<<"distr.first: " << distr.first <<endl;
	}
	*/  

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
		return false;
	}


	//read parameters of this paramDistribution
	for(int j = 0; j< params_num; j++ )
	{
		double param = xmlhandler.getInputParamDistParamValue(root, stream_name, i, j);
		distr.second.push_back(param);
	}

	//check whether the distribution is valid
	if(RandomNumGenerator::checkDistribution(distr) == true)
		return true;
	else
	{
		ERROR << "Invalid Distribution";
		return false;
	}

}

bool WGServer::readAttributeDef(
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

	/*
	//temp
	if(show_process)
	{
	cout<<"attribute type: " << attr_def.attribute_data_type.type <<endl;
	}
	*/
	int data_length = xmlhandler.getAttrDataLength(root, stream_name, i);

	if (data_length < 0)
	{
		ERROR << "Invalid attribute data length: "
			<< data_length;

		return false;
	}

	attr_def.attribute_data_type.length = data_length;

	if(show_process)
	{
		cout<<"the length of the attribute data type: " << attr_def.attribute_data_type.length <<endl;
	}

	//read distribution
	int attr_dist_type = xmlhandler.getAttrDistTypeInt(root, stream_name, i);
	if (attr_dist_type > MAX_DISTRIBUTION_TYPE_NUMBER || attr_dist_type < 0)
	{
		ERROR << "Invalid distribution type: "
			<< attr_dist_type;

	}
	attr_def.source_type = static_cast<distributiontype_t>(attr_dist_type);

	/*
	//temp
	if(show_process)
	{
	cout<<"attr_def.source_type: " << attr_def.source_type <<endl;
	}
	*/
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
		return false;
	}



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


bool WGServer::readAttrParamDist(
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

	/*
	//temp
	if(show_process)
	{
	cout<<"distr.first: " << distr.first <<endl;
	}
	*/


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


	//read parameters of this paramDistribution
	for(int k = 0; k< params_num; k++ )
	{
		double param = xmlhandler.getAttrParamDistParamValue(root, stream_name, attr_i, param_j, k);
		distr.second.push_back(param);
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

bool WGServer::readLine(fstream& file, char* buf, int len)
{
	if (file.eof())
	{
		ERROR << "Invalid file: incomplete information.";
		return false;
	}
	file.getline(buf, len);
	while (!file.eof() && (strlen(buf) == 0 || buf[0] == '%'))
		file.getline(buf, len);

	if (!file.eof())
		return true;
	else
	{
		ERROR << "Invalid file: incomplete information.";
		return false;
	}
}

BOREALIS_NAMESPACE_END



