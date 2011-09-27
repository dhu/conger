#include "Wrapper.h"
#include "TQuery.h"
#include "MedusaClient.h"

#include "rpc_QOSOptimizer.h"

using namespace std;
using namespace tiny_dbwrapper;

Status print_event(ptr<StreamEvent> event)
{
  //NOTICE << "Received event " << *event;
  NOTICE << "Received an event\n";
    return true;
}

TQuery* currquery;
TinyDBWrapper::Wrapper* test_wrapper;
NMSTL::Remote<Borealis::QOSOptimizer>* optimizer;

/*void switchCallback( TQuery* query1, TQuery* query2, Borealis::MedusaClient* pclient, RPC<void> result )
{
  if ( !result.valid())
    cout << "Failure of callback stuff " << result.stat();
  else
    cout << "Operation do callback succeeded " << result << endl;

  NOTICE << " **************************************************  He he !!!";

  //NOTICE << currquery << " " << query1 << " " << query2 << " " << (int)query1->GetState() << " " << (int) query2->GetState();
  //if (currquery == query1)
  {
    query1->Stop();
  //cout << "curr state is: " << (int)query2->GetState() << endl;
    currquery = query2;
    //  if (query2->GetState() == 3)
    //  query2 = testWrapper->RePrepQuery(query2);
    query2->Run(pclient);
    //}
    //if (currquery == query2)
    {
    //query2->Stop();
  //cout << "curr state is: " << (int)query2->GetState() << endl;
    //currquery = query1;
    //if (query1->GetState() == 3)
    // query1 = testWrapper->RePrepQuery(query1);
    //query1->Run(pclient);
    //}
    optimizer->check_switchLoc( wrap(&switchCallback,query1,query2,pclient) );

    }*/

void switch_callback( TQuery* query1, TQuery* query2, Borealis::MedusaClient* pclient)
{
  NMSTL::RPC<int> result = optimizer->check_switch_loc();

  if ( !result.valid())
    cout << "Failure of callback " << result.stat();
  else
    cout << "Operation do callback succeeded " << *result << endl;

  if (*result == 1)
  {
    NOTICE << " **************************************************  He he !!!";

    //NOTICE << currquery << " " << query1 << " " << query2 << " " << (int)query1->GetState() << " " << (int) query2->GetState();
    if (currquery == query1)
    {
      query1->stop();
      cout << "curr state is: " << (int)query2->get_state() << endl;
      currquery = query2;
      if (query2->get_state() == 3)
    query2 = test_wrapper->re_prep_query(query2);
      query2->run(pclient);
    }
    else if (currquery == query2)
    {
      query2->stop();
      cout << "curr state is: " << (int)query2->get_state() << endl;
      currquery = query1;
      if (query1->get_state() == 3)
    query1 = test_wrapper->re_prep_query(query1);
      query1->run(pclient);
      }
  }
  (new CallbackTimer(pclient->get_loop(),wrap(&switch_callback,query1, query2, pclient)))->arm(Time::now()+Time::msecs(3000));

}

void central_callback(Borealis::MedusaClient* pclient)
{

  // Perform an operation
  if (currquery->get_current_score_vector().at(0) != -1)
  {
    NMSTL::RPC<void> result = optimizer->do_update(currquery->get_current_score_vector().at(0), currquery->get_estimate_score_vector().at(0));

    if ( !result.valid())
      cout << "Failure: " << result.stat();
    else
      cout << "Operation succeeded " << result << endl;

    cout << "Estimation: " << currquery->get_estimate_score_vector().at(0) << endl;
    cout << "Current: " << currquery->get_current_score_vector().at(0) << endl;
  }


  (new CallbackTimer(pclient->get_loop(),wrap(&central_callback,pclient)))->arm(Time::now()+Time::msecs(2000));

}

int main(int argc, char *argv[])
{
  // init wrapper

  test_wrapper = (new Wrapper());

  if (!test_wrapper->Initialize())
    {
      cerr << "Error initialize" << endl;
      return -1;
    }

  // init test client

  // The Borealis node that we are going to talk to
  string ip = "127.0.0.1";
  int port = 15000;

  string my_ip = "127.0.0.1";
  int my_port = 25000;

  Borealis::MedusaClient* pclient (new Borealis::MedusaClient(InetAddress(ip,port)));

  Borealis::MedusaID node_id("WrapperTest");

  string schema_sensor_xml = string() +
    "<schema name=\"medusa://output1schema/hehe\">\n" +
    " <field name=\"epoch\" type=\"int\"/>\n" +
    " <field name=\"nodeid\" type=\"int\"/>\n" +
    " <field name=\"ts\" type=\"int\"/>\n" +
    " <field name=\"light\" type=\"int\"/>\n" +
    "</schema>\n";

    Borealis::Schema sensor;
    Status s1 = sensor.from_xml(schema_sensor_xml);

    if (!s1)
    {
        cerr << "Failed creating schema" << s1 << endl;
        return -1;
    }

    NOTICE << "Registering " << sensor;
    RPC<void> result = pclient->create_schema(sensor);
    NOTICE << "create_schema: " << result;
    if (!result.valid())
    {
        cerr << result.stat() << endl;
        return -1;
    }

    string schema_xml2 = string() +
        "<schema name=\"factory\">\n" +
        " <field name=\"tsval\" type=\"int\"/>\n" +
        " <field name=\"tempval\" type=\"int\"/>\n" +
        "</schema>\n";
    Borealis::Schema schema2;
    Status s2 = schema2.from_xml(schema_xml2);
    if ( !s2 )
        FATAL << "Failed parsing schema from xml" << s2;

    NOTICE << "Registering " << schema2;
    result = pclient->create_schema(schema2);
    NOTICE << "create_schema: " << result;
    if (!result.stat()) return 1;

    Borealis::StreamDef sensors("medusa://output1/127.0.0.1:15000", sensor);

    NOTICE << "Registering " << sensors;
    result = pclient->create_stream(sensors);
    NOTICE << "create_stream: " << result;
    if (!result.valid())
    {
        cerr << result.stat() << endl;
        return -1;

    }
    string input_stream_name2 = "medusa://output3/127.0.0.1:15000";
    Borealis::StreamDef input_stream_def2(input_stream_name2, schema2);
    NOTICE << "Registering " << input_stream_def2;
    result = pclient->create_stream(input_stream_def2);
    NOTICE << "create_stream: " << result;
    if (!result.stat()) return 1;



    cout << "Successfully registered all schemas and streams" << endl;

    // wrapper tests


    cout << "Getting Catalog..." << endl;

    cout << "Catalog as XML:" << endl;
    cout << test_wrapper->get_catalog_xml() << endl;

    cout << "Catalog as object instances:" << endl;
    TinyDBWrapper::Catalog* test_catalog = test_wrapper->get_catalog();
    for (unsigned int i=0; i<test_catalog->get_schema_count(); i++)
    {
        TinyDBWrapper::Schema* test_schema = test_catalog->get_schema(i);
        cout << test_schema->get_name() << " (";
        for (unsigned int j=0; j<test_schema->get_field_count(); j++)
        {
            TinyDBWrapper::Field* test_field = test_schema->get_field(j);
            cout << test_field->get_name() << "(" << test_field->get_cost() << "), ";
        }
        cout << ");" << endl;
    }

    cout << "Getting available Boxes..." << endl;
    for (unsigned int i=0; i<test_catalog->get_box_count(); i++)
    {
        TinyDBWrapper::Box* test_box = test_catalog->get_box(i);
        cout << "Box as XML:" << endl << test_box->get_xml() << endl;
        cout << "Box as object instance:" << endl << test_box->get_name() << ": " << test_box->get_type() << "" << endl << endl;
    }

    cout << "Preparing Test Query ..." << endl;
    string str_query_1 = string() +
      "<query name=\"test_query1\">\n" +
      /*        " <box name=\"my_map\" type=\"map\">\n" +
            "  <input port=\"1\" stream=\"sensors-stream\"/>\n" +
            "  <output port=\"1\" stream=\"output1\"/>\n" +
            "  <param name=\"output-field-name.0\" value=\"sensors.nodeid\"/>\n"  +
            "  <param name=\"expression.0\" value=\"sensors.nodeid\"/>\n"  +
            "  <param name=\"output-field-name.1\" value=\"sensors.light\"/>\n"  +
            "  <param name=\"expression.1\" value=\"sensors.light\"/>\n"  +
            "  <param name=\"output-field-name.2\" value=\"sensors.temp\"/>\n"  +
            "  <param name=\"expression.2\" value=\"sensors.temp\"/>\n"  +
            " </box>\n" +
            " <box name=\"my_filter\" type=\"filter\">\n" +
            "  <input port=\"1\" stream=\"sensors-stream\"/>\n" +
            "  <output port=\"1\" stream=\"output1\"/>\n" +
            "  <param name=\"predicate.0\" value=\"sensors.nodeid > 0\"/>\n"  +
            "  <param name=\"pass-on-false-port\" value=\"0\"/>\n"  +
            " </box>\n" +*/
    " <box name=\"my_map\" type=\"map\">\n" +
      "  <input port=\"1\" stream=\"sensors-stream\"/>\n" +
      "  <output port=\"1\" stream=\"medusa://output1/127.0.0.1:15000\"/>\n" +
      "  <param name=\"output-field-name.0\" value=\"sensors.epoch\"/>\n"  +
      "  <param name=\"expression.0\" value=\"sensors.epoch\"/>\n"  +
      "  <param name=\"output-field-name.1\" value=\"sensors.nodeid\"/>\n"  +
      "  <param name=\"expression.1\" value=\"sensors.nodeid\"/>\n"  +
      "  <param name=\"output-field-name.2\" value=\"sensors.ts\"/>\n"  +
      "  <param name=\"expression.2\" value=\"sensors.ts\"/>\n"  +
      "  <param name=\"output-field-name.3\" value=\"sensors.light\"/>\n"  +
      "  <param name=\"expression.3\" value=\"sensors.light\"/>\n"  +
      " </box>\n" +
      "</query>\n";
    string str_query_2 = string() +
      "<query name=\"test_query2\">\n" +
      " <box name=\"my_join\" type=\"joinstatic\">\n" +
      "  <input port=\"1\" stream=\"medusa://output1/127.0.0.1:15000\"/>\n" +
      "  <input port=\"2\" stream=\"medusa://output3/127.0.0.1:15000\"/>\n" +
      "  <output port=\"1\" stream=\"medusa://output2/127.0.0.1:15000\"/>\n" +
      "  <param name=\"predicate\" value=\"left.ts = right.tsval and left.light = right.tempval\"/>\n"  +
      //"  <param name=\"predicate\" value=\"left.ts = right.tsval\"/>\n"  +
      "  <param name=\"out-field.0\" value=\"left.epoch\"/>\n"  +
      "  <param name=\"out-field.1\" value=\"left.nodeid\"/>\n"  +
      "  <param name=\"out-field.2\" value=\"left.ts\"/>\n"  +
      "  <param name=\"out-field.3\" value=\"left.light\"/>\n"  +
      " </box>\n" +
      "</query>\n";
    string str_query_3 = string() +
      "<query name=\"test_query3\">\n" +
      " <box name=\"my_join\" type=\"staticjoin\">\n" +
      "  <input port=\"1\" stream=\"sensors-stream\"/>\n" +
      "  <input port=\"2\" stream=\"fac2\"/>\n" +
      "  <output port=\"1\" stream=\"medusa://output1/127.0.0.1:15000\"/>\n" +
      "  <param name=\"predicate\" value=\"sensors.ts = fac2.tsval and sensors.light = fac2.tempval\"/>\n"  +
      "  <param name=\"out-field.0\" value=\"left.epoch\"/>\n"  +
      "  <param name=\"out-field.1\" value=\"left.nodeid\"/>\n"  +
      "  <param name=\"out-field.2\" value=\"left.ts\"/>\n"  +
      "  <param name=\"out-field.3\" value=\"left.light\"/>\n"  +
      " </box>\n" +
      "</query>\n";
      /*        " <box name=\"my_filter2\" type=\"filter\">\n" +
        "  <input port=\"1\" stream=\"output3\"/>\n" +
        "  <output port=\"1\" stream=\"output4\"/>\n" +
        "  <param name=\"predicate.0\" value=\"sensors.light > 4\"/>\n"  +
        "  <param name=\"pass-on-false-port\" value=\"0\"/>\n"  +
        " </box>\n" +
        " <box name=\"my_aggregate\" type=\"aggregate\">\n" +
        "  <input port=\"1\" stream=\"output4\"/>\n" +
        "  <output port=\"1\" stream=\"output5\"/>\n" +
        "  <param name=\"aggregate-function.0\" value=\"max(sensors.light)\"/>\n"  +
        "  <param name=\"aggregate-function.1\" value=\"avg(sensors.light)\"/>\n"  +
        "  <param name=\"window-size-by\" value=\"VALUES\"/>\n"  +
        "  <param name=\"window-size\" value=\"1\"/>\n"  +
        "  <param name=\"advance\" value=\"1\"/>\n"  +
        "  <param name=\"timeout\" value=\"-1\"/>\n"  +
        "  <param name=\"order-by\" value=\"FIELD\"/>\n"  +
        "  <param name=\"order-on-field\" value=\"sensors.epoch\"/>\n"  +
        "  <param name=\"slack\" value=\"0\"/>\n"  +
        "  <param name=\"group-by\" value=\"sensors.nodeid, sensors.parent, relation.light\"/>\n"  +
        " </box>\n" +
        "</query>\n";*/
    /*

    string query_xml2 = string() +
      "<query name=\"test2\">\n" +

      // For every 1 seconds count the number of packets
      " <box name=\"myfilter\" type=\"filter\">\n" +
      "  <input port=\"1\" stream=\"medusa://output1/127.0.0.1:15000\"/>\n" +
      "  <output port=\"1\" stream=\"medusa://output2/127.0.0.1:15000\"/>\n" +
      "  <param name=\"expression.0\" value=\"1=1\"/>\n"  +
      " </box>\n" +

      "</query>\n";
    */
    result = pclient->create_query_xml(str_query_2);
    NOTICE << "create_query_xml: " << result;
    if (!result.stat()) return false;
    NOTICE << "Starting queries";
    result = pclient->start_query("test_query2");
    NOTICE << "start_query: " << result;
    if (!result.stat()) return false;

    TQuery* ptest_query_1;
    TQuery* ptest_query_2;

    ptest_query_2 = test_wrapper->prepare_query(str_query_3, 2048);
    cout << "curr state is: " << (int)ptest_query_2->get_state() << endl;
    if ((ptest_query_1 = test_wrapper->prepare_query(str_query_1, 2048)) != NULL)
    {
      ptest_query_2 = test_wrapper->get_queries()->get_query(0);
      cout << "curr state1 is: " << (int)ptest_query_2->get_state() << endl;
        cout << "Success" << endl;
        cout << "Query:" << endl;
        cout << ptest_query_1->get_tiny_dbquery() << endl;
        cout << "curr state2 is: " << (int)ptest_query_2->get_state() << endl;
        cout << "Starting query..." << endl;

        //NMSTL::IOEventLoop loop = pclient->get_loop();

        // Create an endpoint capable of communicating with the server
        NMSTL::TCPRPCEndpoint my_endpoint(pclient->get_loop(), NMSTL::TCPSocket(NMSTL::InetAddress(ip,port)));

        // Get a handle to the remote calculator
        optimizer = (new NMSTL::Remote<Borealis::QOSOptimizer>(my_endpoint.get<Borealis::QOSOptimizer>("QOSOptimizer")));

        cout << "curr state3 is: " << (int)ptest_query_2->get_state() << endl;

        //optimizer->check_switchLoc( wrap(&switchCallback,ptestQuery1,ptestQuery2,pclient) );
        cout << "curr state4 is: " << (int)ptest_query_2->get_state() << endl;

        currquery = ptest_query_1;
        central_callback(pclient);
        switch_callback(ptest_query_1, ptest_query_2, pclient);

        if (! pclient->set_data_path(64000,ip,port))
        {
          FATAL << "Failed setting data path";
        }
        cout << "curr state is: " << (int)ptest_query_2->get_state() << endl;

        if (ptest_query_1->run(pclient))
        {
          cout << "curr state is: " << (int)ptest_query_2->get_state() << endl;

          Status stat = pclient->set_data_handler(InetAddress(my_ip,my_port),wrap(&print_event));
          if ( stat )
          {
            stat = pclient->fast_subscribe("medusa://output2/127.0.0.1:15000");
          }
          NOTICE << "Done subscribing";

          pclient->run();

        }
        else
          {
            cout << "Failure" << endl;
          }
    }
    else
      {
        cout << "Failure" << endl;
      }

    cout << "exiting..." << endl;

    test_wrapper->deinitialize();

    return 0;
}
