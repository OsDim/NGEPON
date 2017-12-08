/*
 * LocalTraffic.cc
 *
 *  Created on: 2014/2/25
 *      Author: chienson
 */


#include <omnetpp.h>
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "common/MPCP_codes.h"
#include <fstream>
#include <vector>

using namespace std;
class LocalNetwork : public cSimpleModule
{
    public:
        LocalNetwork();
        ~LocalNetwork();
    private:
        double offered_load, max_rate, highPriorityRatio;
        double send_rate;
        double pktArrivalRate;
        int pktCount, sizeOfONU;
        cMessage *sendMessageEvent,
                 *sendTrafficEvent,
                 *nextArrivalEvent,
                 *triggerEvent;

        cQueue txQueue;
        vector<uint32_t> onuUpBytes;

        cModule *epon = simulation.getModuleByPath("EPON");

//    ------------------------poisson------------------------------
        double local_throughput, interpacket_time;
        bool trafficPoisson;


//    ------------------------self similar------------------------------
        bool pareto_on, self_similar, asymmetric_flow ;
        double pareto_rate, next_switch, inter_frame_gap, scale_on, scale_off, t_on, t_off,
               alpha_on, alpha_off, beta_on, beta_off, coef_on, coef_off, multiple_of_flow;
        uint16_t packet_train_length, mean_frame_size,
                 on_packet_train_length, off_packet_train_length;

      void generateDataFrame();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(LocalNetwork);

LocalNetwork::LocalNetwork(){
    sendMessageEvent = NULL;
    sendTrafficEvent = NULL;
    nextArrivalEvent = NULL;
    triggerEvent = NULL;

    pktArrivalRate=0;
    pktCount=0;
}

LocalNetwork::~LocalNetwork(){
    cancelAndDelete(sendMessageEvent);
    cancelAndDelete(sendTrafficEvent);
    cancelAndDelete(nextArrivalEvent);
    cancelAndDelete(triggerEvent);

    txQueue.clear();
}

void LocalNetwork::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    sendTrafficEvent = new cMessage("sendTrafficEvent");
    nextArrivalEvent = new cMessage("nextArrivalEvent");

    scheduleAt(simTime(), nextArrivalEvent);

    offered_load = par("offered_load");// 0.01
    highPriorityRatio = par("high_priority_ratio");
    sizeOfONU = epon->par("sizeOfONU").longValue();
    send_rate = par("rate");//1e-10? mean 1*10^-10
    max_rate = 1 / send_rate;// 1/0.0000000001 = 10000000000 = 10Gbit

//    ------------------------poisson------------------------------
    trafficPoisson = par("trafficPoisson");

    local_throughput = offered_load * max_rate; // 0.01 * 10Gbit = 100Mbit
    interpacket_time = ((((MIN_FRAME_LEN + MAX_FRAME_LEN) / 2) * 8) / local_throughput);


//    ------------------------self similar------------------------------
    triggerEvent = new cMessage("triggerEvent");
    scheduleAt(simTime(), triggerEvent);

    pareto_rate = par("pareto_rate");
    alpha_on = par("pareto_alpha_on");
    alpha_off = par("pareto_alpha_off");
    beta_on = par("pareto_beta_on");

    self_similar = !trafficPoisson;
    mean_frame_size = (MIN_FRAME_LEN + MAX_FRAME_LEN / 2.0) * 8.0;
    inter_frame_gap = mean_frame_size / max_rate ;
    coef_on = pow(((1.19 * alpha_on) - 1.166), -0.027);
    coef_off = pow(((1.19 * alpha_off) - 1.166), -0.027);
    asymmetric_flow = epon->par("asymmetric_flow").boolValue() ;
    multiple_of_flow = epon->par("multiple_of_flow").doubleValue() ;
    t_on = (alpha_on - 1) / alpha_on;
    t_off = (alpha_off - 1) / alpha_off;
    scale_on = 1 - pow(DBL_MIN, t_on);
    scale_off = 1 - pow(DBL_MIN, t_off);
    beta_off = (coef_on / coef_off) * (t_off / t_on) * (scale_on / scale_off) * ((1 / offered_load) - 1);
    pareto_on = false;

    onuUpBytes.push_back(0);
}

void LocalNetwork::handleMessage(cMessage *msg)
{
    if (offered_load==0) return;

    if (msg == nextArrivalEvent)
    {
        if (self_similar && pareto_on && on_packet_train_length-- >0)
        {
            generateDataFrame();
            if ( asymmetric_flow )
              scheduleAt(simTime() + inter_frame_gap / 2.8 , nextArrivalEvent);
            else
            scheduleAt(simTime() + inter_frame_gap , nextArrivalEvent);
        }
        else if (trafficPoisson)
        {
            generateDataFrame();
            scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
        }

        // ---- packet arrival rate
        if (trafficPoisson)
            pktArrivalRate += exponential(interpacket_time, this->getIndex());
        if (self_similar)
//            pktArrivalRate +=pareto_shifted(alpha_on, beta_on, 0, this->getIndex() )*inter_frame_gap;
            pktArrivalRate += inter_frame_gap; // no use

        pktCount++;
        // ----
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }

    else if (msg == triggerEvent)
    {
        if (self_similar)
        {
            pareto_on = !pareto_on;

            if (pareto_on)
            {
                on_packet_train_length = round(pareto_shifted(alpha_on, beta_on, 0, this->getIndex() ));

                if ( asymmetric_flow ) {
                  if ( this->getIndex() < 8 )
                    next_switch = on_packet_train_length * (inter_frame_gap / 2.8 ) ;
                  else next_switch = on_packet_train_length * inter_frame_gap * 4;
                }
                else
                  next_switch = on_packet_train_length * inter_frame_gap ;
                scheduleAt(simTime(), nextArrivalEvent);
            }
            else
            {
                cancelEvent(nextArrivalEvent);
                off_packet_train_length = round(pareto_shifted(alpha_off, beta_off, 0, this->getIndex() ));

                if ( asymmetric_flow ) {
                    if ( this->getIndex() < 8 )
                      next_switch = off_packet_train_length * (inter_frame_gap / 2.8 );
                    else next_switch = off_packet_train_length * inter_frame_gap * 4 ;
                }
                else

                  next_switch = off_packet_train_length * inter_frame_gap ;
            }
            scheduleAt(simTime()+ next_switch, msg);
        }
        else if (trafficPoisson)
            cancelEvent(triggerEvent);

        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }
    else if (msg == sendTrafficEvent)
    {
        if (txQueue.empty())
            return;
        else
        {
            MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
            send(pkt, "ethUp$o");
            scheduleAt(simTime()+pkt->getBitLength()*send_rate, sendTrafficEvent);
        }
    }

}


void LocalNetwork::generateDataFrame()
{
    MyPacket *job = new MyPacket("traffic");
    job->setTimestamp();
    int16_t hpRatio = highPriorityRatio*1000;               // High priority percentage 0.05 * 1000 = 50

    uint32_t len = intuniform(MIN_FRAME_LEN, MAX_FRAME_LEN, this->getIndex());
    job->setByteLength(len);
    uint16_t pri = intuniform(1, 1000, this->getIndex());         // 1~100
    onuUpBytes[0]+=len;

    if (pri>1000-hpRatio)   // 1000 - 50 = 950
        job->setPriority(0);
    else
        job->setPriority(1);

    txQueue.insert(job);
}

void LocalNetwork::finish() {
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "LocalNet.txt";
    ofstream out ;
    out.open(path.str().c_str(), ios::out | ios::app);

    out << "ONU[" << this->getIndex() << "] upstream: " << onuUpBytes[0]/pow(2,20)*8 << "M bits" << " avgArrival=" << pktArrivalRate/pktCount << endl;

    cModule *epon = simulation.getModuleByPath("EPON");
    int onuSize = epon->par("sizeOfONU").longValue();
    if (this->getIndex()==onuSize-1)
        out << "\n\n";

    out.close();
}
