#include "DataThread.h"
#include "MedusaClient.h"
#include "args.h"
#include <iostream>
#include <string>
#include "TQuery.h"

using namespace Borealis;
using namespace NMSTL;

typedef void*(THREAD_MAIN_FN)(void*);

struct s_field
{
  string  str_field_name;
  BYTE  b_field_type;
  int       i_field_size;
  long  l_num_value;
  string    str_byte_value;
};

TinyDBWrapper::DataThread::DataThread()
{
  _p_tquery = NULL;
}

TinyDBWrapper::DataThread::~DataThread()
{
  if (_listen_socket.is_valid())
    {
      _listen_socket.close();
    }
  if (_data_socket.is_valid())
    {
      _data_socket.close();
    }
}

bool TinyDBWrapper::DataThread::create(int i_port, TQuery* p_tquery)
{
  int i_ret;

  if (!_listen_socket.create())
    {
      cerr << "DataThread::Create: Create listen socket error" << endl;
      return false;
    }
  if (!_listen_socket.listen(i_port))
    {
      cerr << "DataThread::Create: Listen error" << endl;
      return false;
    }
  _p_tquery = p_tquery;
  if ((i_ret = pthread_create(&_thread, NULL, this->run, this)) != 0)
    {
      _p_tquery = NULL;
      cerr << "DataThread::Create: Create thread error: " << i_ret << endl;
      return false;
    }
  return true;
}

bool TinyDBWrapper::DataThread::cancel()
{
  int i_ret;

  if ((i_ret = pthread_cancel(_thread)) != 0)
    {
      cerr << "DataThread::Cancel: Cancel thread error: " << i_ret << endl;
      return false;
    }
  if (_listen_socket.is_valid())
    {
      _listen_socket.close();
    }
  if (_data_socket.is_valid())
    {
      _data_socket.close();
    }
  _p_tquery = NULL;
  return true;
}

void* TinyDBWrapper::DataThread::run(void* p_param)
{
  DataThread* p_this = (DataThread*)p_param;

  if (!p_this->_listen_socket.accept(p_this->_data_socket))
    {
      cerr << "DataThread::run Accept error" << endl;
      return NULL;
    }

  vector<s_field> v_schema;
  // first read stream schema
  int i_attr_count = p_this->_data_socket.read_int();
  int i_size = 0;
  BYTE* b_buffer;
  cerr << i_attr_count << "Attributes " << endl;
  for (int i=0; i<i_attr_count; i++)
    {
      s_field new_field;
      i_size = p_this->_data_socket.read_int();
      b_buffer = new BYTE[i_size + 1];
      p_this->_data_socket.read_byte_array(b_buffer, i_size);
      b_buffer[i_size] = '\0';
      new_field.str_field_name = (char*)b_buffer;
      new_field.b_field_type = p_this->_data_socket.read_byte();
      new_field.i_field_size = p_this->_data_socket.read_int();
      cerr << i << ": " << new_field.str_field_name << "(" << (int)new_field.b_field_type << ":" << new_field.i_field_size << ")" << endl;
      v_schema.insert(v_schema.end(), new_field);
    }
  while(true)
    {
      if (p_this->_data_socket.read_byte() == 0)
    {
      // receiving tuples
      for (int i=0; i<i_attr_count; i++)
        {
          switch (v_schema.at(i).i_field_size)
        {
        case 0:
          i_size = p_this->_data_socket.read_int();
          b_buffer = new BYTE[i_size + 1];
          p_this->_data_socket.read_byte_array(b_buffer, i_size);
          b_buffer[i_size] = '\0';
          v_schema.at(i).str_byte_value = (char*)b_buffer;
          delete [] b_buffer;
          break;
        case 1:
          v_schema.at(i).l_num_value = p_this->_data_socket.read_byte();
          break;
        case 2:
          v_schema.at(i).l_num_value = p_this->_data_socket.read_short();
          break;
        case 4:
          v_schema.at(i).l_num_value = p_this->_data_socket.read_int();
          break;
        case 8:
          b_buffer = new BYTE[9];
          p_this->_data_socket.read_byte_array(b_buffer, 8);
          b_buffer[8] = '\0';
          v_schema.at(i).str_byte_value = (char*)b_buffer;
          delete [] b_buffer;
          break;
        }
        }

      string new_tuple;
      for (unsigned i=0; i<v_schema.size(); i++)
        {
          if (v_schema.at(i).i_field_size == 8)
        {
          new_tuple += v_schema.at(i).str_byte_value;
          cout << v_schema.at(i).str_byte_value << "(8)\t| ";
        }
          else
        {
          new_tuple += string((const char *)(&(v_schema.at(i).l_num_value)),
                     sizeof((int)v_schema.at(i).l_num_value));
          cout << v_schema.at(i).l_num_value << "("
               << v_schema.at(i).i_field_size << ")\t| ";
        }
        }
      ptr<StreamEvent> event(new StreamEvent(p_this->_p_tquery->get_query_stream(), new_tuple, v_schema.size(), true));
      cout << endl;
      // send tuple
      bool f_repeat = false;
      do
      {
        f_repeat = false;
        Status stat = p_this->_p_tquery->_p_client->fast_post_event(event);
        if (stat)
          {
        // All ok
          }
        else if (stat.as_string() == DataHandler::NO_SPACE)
        {
          sleep(50); // Wait if no more space in buffer
          f_repeat = true;
        }
        else
          {
        cerr << "Error: Connection closed" << endl;
          }
      } while (f_repeat);
    }
      else
    {
      // receiving VM and setting current values, hard coded
      unsigned int i_no_of_values = (unsigned int)p_this->_data_socket.read_int();
      double d_temp;
      cout << "VM: [";
      if (i_no_of_values == p_this->_p_tquery->get_scores()->get_metric_count())
        {
          for (unsigned int i=0; i<p_this->_p_tquery->get_scores()->get_metric_count(); i++)
        {
          d_temp = p_this->_data_socket.read_double();
          p_this->_p_tquery->get_scores()->get_metric(i)->set_current_value(d_temp);
          if (i<p_this->_p_tquery->get_scores()->get_metric_count()-1)
            {
              cout << p_this->_p_tquery->get_scores()->get_metric(i)->get_name() << " = " << d_temp << ", ";
            }
          else
            {
              cout << p_this->_p_tquery->get_scores()->get_metric(i)->get_name() << " = " << d_temp << "]" << endl;
            }
        }
        }
      else
        {
          cerr << "Error: wrong number of VM elements" << endl;
        }

    }
    }
  return NULL;
}
