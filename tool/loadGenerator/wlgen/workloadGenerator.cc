#include <common.h>

#include "WorkloadGenerator.h"
#include "WGServer.h"
#include <iostream>			// for cout
#include <sstream>			// for istringstream

using namespace Borealis;

int main(int argc, char* argv[])
{
	WorkloadGenerator workload_generator;

	if (argc == 1) 
	{

		// start gui command receiver
		WGServer	gui_command_reciever(&workload_generator);
		if (gui_command_reciever.initialize())
			workload_generator.startCommandReceiver();
		else 
		{
			WARN << "Initializing GUI command reciver failed.";
			return 1;
		}
	} 
	else if (argc == 3) 
	{
		// arguments: 
		//            workload_generator_server_port,
		//            number_of_handler_threads
		istringstream buffer2(argv[3]);
		int workload_generator_server_port;
		if (!(buffer2 >> workload_generator_server_port)) 
		{
			cout << "Usage: wlgen "
				<< "[aurora_input_server_name aurora_input_server_port] "
				<< "[GUI_command_receiver_port number_of_handler_threads]"
				<< endl;
			return 1;
		}
		istringstream buffer3(argv[4]);
		int number_of_handler_threads;
		if (!(buffer3 >> number_of_handler_threads)) 
		{
			cout << "Usage: wlgen "
				<< "[aurora_input_server_name aurora_input_server_port] "
				<< "[GUI_command_receiver_port number_of_handler_threads]"
				<< endl;
			return 1;
		}

		WGServer	gui_command_reciever(
			&workload_generator, 
			workload_generator_server_port,
			number_of_handler_threads);
		if (gui_command_reciever.initialize())
			workload_generator.startCommandReceiver();
		else 
		{
			WARN << "Initializing GUI command reciver failed.";
			return 1;
		}
	}
	else 
	{
		cout << "Usage: wlgen "
			<< "[GUI_command_receiver_port number_of_handler_threads]" 
			<< endl;
		return 1;
	}

	return 0;
}
