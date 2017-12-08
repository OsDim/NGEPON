/*
 * TrafficGen.cc
 *
 *  Created on: 2014/6/24
 *      Author: chienson
 */


#include <omnetpp.h>
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "common/MPCP_codes.h"
#include <fstream>
#include <vector>

using namespace std;
class TrafficGen : public cSimpleModule
{
    public:
        TrafficGen();
        ~TrafficGen();
    private:
        double offered_load, max_rate, highPriorityRatio, send_rate;
        int sizeOfONU;

        cMessage *sendMessageEvent,
                 *sendTrafficEvent,
                 *nextArrivalEvent,
                 *triggerEvent;

        cQueue txQueue;
        vector<uint32_t> onuDownBytes;

        cModule *epon = simulation.getModuleByPath("EPON");
        cSimpleModule *trafficGen;

//    ------------------------poisson------------------------------
        double  local_throughput, interpacket_time;
        bool trafficPoisson;

//    ------------------------self similar------------------------------
        bool pareto_on, self_similar, asymmetric_flow;
        uint16_t packet_train_length, mean_frame_size,
                 on_packet_train_length, off_packet_train_length;
        double pareto_rate, next_switch, inter_frame_gap, scale_on, scale_off, t_on, t_off,
                   alpha_on, alpha_off, beta_on, beta_off, coef_on, coef_off, multiple_of_flow;

        uint16_t max_packet_length;

        void generateDataFrame();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(TrafficGen);

TrafficGen::TrafficGen(){
    sendMessageEvent = NULL;
    sendTrafficEvent = NULL;
    nextArrivalEvent = NULL;
    triggerEvent = NULL;
}

TrafficGen::~TrafficGen(){
    cancelAndDelete(sendMessageEvent);
    cancelAndDelete(sendTrafficEvent);
    cancelAndDelete(nextArrivalEvent);
    cancelAndDelete(triggerEvent);

    txQueue.clear();
}

void TrafficGen::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    sendTrafficEvent = new cMessage("sendTrafficEvent");
    nextArrivalEvent = new cMessage("nextArrivalEvent");
    scheduleAt(simTime(), nextArrivalEvent);

    offered_load = par("offered_load");
    highPriorityRatio = par("high_priority_ratio");
    send_rate = par("rate");
    sizeOfONU = epon->par("sizeOfONU").longValue();

    max_rate = 1 / send_rate;
    onuDownBytes.push_back(0);

//    ------------------------poisson------------------------------
    trafficPoisson = par("trafficPoisson");

    local_throughput = offered_load * max_rate;
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
    inter_frame_gap = mean_frame_size / max_rate;
    asymmetric_flow = epon->par("asymmetric_flow").boolValue() ;
    multiple_of_flow = epon->par("multiple_of_flow").doubleValue() ;
    coef_on = pow(((1.19 * alpha_on) - 1.166), -0.027);
    coef_off = pow(((1.19 * alpha_off) - 1.166), -0.027);
    t_on = (alpha_on - 1) / alpha_on;
    t_off = (alpha_off - 1) / alpha_off;
    scale_on = 1 - pow(DBL_MIN, t_on);
    scale_off = 1 - pow(DBL_MIN, t_off);
    beta_off = (coef_on / coef_off) * (t_off / t_on) * (scale_on / scale_off) * ((1 / offered_load) - 1);
    pareto_on = false;
}

void TrafficGen::handleMessage(cMessage *msg)
{
    if (offered_load==0) return;

    if (msg == nextArrivalEvent)
    {
        if (self_similar && pareto_on && on_packet_train_length-- >0)
        {
            generateDataFrame();
            if ( asymmetric_flow )
              //scheduleAt(simTime() + inter_frame_gap / multiple_of_flow , nextArrivalEvent);
              //scheduleAt(simTime() + inter_frame_gap / 2.66 , nextArrivalEvent);
                scheduleAt(simTime() + inter_frame_gap / 2.8 , nextArrivalEvent);
            else
              scheduleAt(simTime() + inter_frame_gap , nextArrivalEvent);
        }
        else if (trafficPoisson)
        {
            generateDataFrame();
            scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
        }

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
                    //next_switch = on_packet_train_length * (inter_frame_gap / multiple_of_flow) ;
                  //else next_switch = on_packet_train_length * inter_frame_gap * multiple_of_flow ;
                   //next_switch = on_packet_train_length * (inter_frame_gap / 2.66 ) ;
                    next_switch = on_packet_train_length * (inter_frame_gap / 2.8 ) ;
                  else next_switch = on_packet_train_length * inter_frame_gap *4 ;

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
                    //next_switch = off_packet_train_length * (inter_frame_gap / multiple_of_flow);
                  //else next_switch = off_packet_train_length * inter_frame_gap * multiple_of_flow ;
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
            sendDirect(pkt, simulation.getModuleByPath("EPON.olt.dba"), "gen");
            scheduleAt(simTime()+pkt->getBitLength()*send_rate, sendTrafficEvent);
        }
    }

}

void TrafficGen::generateDataFrame()
{
    MyPacket *job = new MyPacket("traffic");
    job->setTimestamp();
    job->setDestAddr(this->getIndex()+2);

    int16_t hpRatio = highPriorityRatio*100;               // High priority percentage 50%

    uint32_t len = intuniform(MIN_FRAME_LEN, MAX_FRAME_LEN, this->getIndex());
    job->setByteLength(len);

    uint16_t pri = intuniform(1, 100, this->getIndex());         // 0~99

    onuDownBytes[0]+=len;

    if (pri>100-hpRatio)
        job->setPriority(0);
    else
        job->setPriority(1);

    txQueue.insert(job);
}

void TrafficGen::finish() {
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "TrafficGen.txt";
    ofstream out ;
    out.open(path.str().c_str(), ios::out | ios::app);

    out << "OFFER_LOAD : " << offered_load << endl ;
    out << "ONU[" << this->getIndex() << "] downstream: " << onuDownBytes[0]/pow(2,20)*8 << "M bits" << endl;

    cModule *epon = simulation.getModuleByPath("EPON");
    int onuSize = epon->par("sizeOfONU").longValue();
    if (this->getIndex()==onuSize-1)
        out << "\n\n";

    out.close();
}
