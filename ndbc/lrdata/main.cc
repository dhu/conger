
#include "common.h"
#include "DataProvider.h"
#include <iostream>
#include <list>
#include "MedusaClient.h"
#include "NMSTL/debug"
#include "NMSTL/ptr"
#include "TrafficInputTuple.h"

using namespace ndbc;
using namespace Borealis;

DataProvider *provider;
MedusaClient *medusaClient;

void send_packet()
{
    list<TrafficTuple> tuples = provider->get_data();
    ptr<StreamEvent> eventPacket;
    while (!tuples.empty())
    {
        eventPacket.reset();
        eventPacket = ptr<StreamEvent>(new StreamEvent("positionreport"));
        eventPacket->_inject = true;
        TrafficTuple tuple = tuples.front();
        tuples.pop_front();
        /* XXX 对于速度是 -1 的元祖，我们直接丢失 */
        if (tuple.m_iSpeed == -1)
        {
            continue;
        }

        PositionReportTuple position_report;
        position_report.m_iTime = tuple.m_iTime;
        position_report.m_iSpeed = tuple.m_iSpeed;
        position_report.m_iDir = tuple.m_iDir;
        position_report.m_iLane = tuple.m_iLane;
        position_report.m_iPos = tuple.m_iPos;
        position_report.m_iSeg = tuple.m_iSeg;
        position_report.m_iVid = tuple.m_iVid;
        position_report.m_iXway = tuple.m_iXway;

        string stringData(reinterpret_cast<const char *>(&position_report),
                sizeof(PositionReportTuple));
        DEBUG << "data size:" << stringData.size();
        eventPacket->insert_bin(stringData);
        medusaClient->fast_post_event(eventPacket);
        DEBUG << "sending " << "time： " << position_report.m_iTime << " vid："
                << position_report.m_iVid;
    }

    (new CallbackTimer(medusaClient->get_loop(),
            wrap(&send_packet)))->arm(
            Time::now() + Time::msecs(1000));

}

void enqueue()
{
    medusaClient = new MedusaClient(InetAddress());
    Status status = medusaClient->set_data_path(676110,
            Util::get_host_address("127.0.0.1"), 15000);
    if (!status)
    {
        WARN << "set data path error...";
        exit(errno);
    }
    DEBUG << "set_data_path status: " << status;

    provider = new DataProvider("./cardatapoints.out");
    send_packet();

    medusaClient->run();
    INFO << "data sent...";
}

int main(char ** args)
{
    enqueue();
    return 0;
}
