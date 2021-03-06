//
// Generated file, do not edit! Created by nedtool 4.6 from traffic_gen/Messages/EPON_messages.msg.
//

#ifndef _EPON_MESSAGES_M_H_
#define _EPON_MESSAGES_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include <inttypes.h> 
// }}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:23</tt> by nedtool.
 * <pre>
 * packet MyPacket
 * {
 *     uint16_t SrcAddr;
 *     uint16_t DestAddr;
 *     uint16_t Priority;
 *     simtime_t txEnd;
 *     simtime_t txStart;
 * 
 *     bool lastPkt = false;
 *     uint32_t cycleTag;
 * }
 * </pre>
 */
class MyPacket : public ::cPacket
{
  protected:
    uint16_t SrcAddr_var;
    uint16_t DestAddr_var;
    uint16_t Priority_var;
    simtime_t txEnd_var;
    simtime_t txStart_var;
    bool lastPkt_var;
    uint32_t cycleTag_var;

  private:
    void copy(const MyPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MyPacket&);

  public:
    MyPacket(const char *name=NULL, int kind=0);
    MyPacket(const MyPacket& other);
    virtual ~MyPacket();
    MyPacket& operator=(const MyPacket& other);
    virtual MyPacket *dup() const {return new MyPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual uint16_t getSrcAddr() const;
    virtual void setSrcAddr(uint16_t SrcAddr);
    virtual uint16_t getDestAddr() const;
    virtual void setDestAddr(uint16_t DestAddr);
    virtual uint16_t getPriority() const;
    virtual void setPriority(uint16_t Priority);
    virtual simtime_t getTxEnd() const;
    virtual void setTxEnd(simtime_t txEnd);
    virtual simtime_t getTxStart() const;
    virtual void setTxStart(simtime_t txStart);
    virtual bool getLastPkt() const;
    virtual void setLastPkt(bool lastPkt);
    virtual uint32_t getCycleTag() const;
    virtual void setCycleTag(uint32_t cycleTag);
};

inline void doPacking(cCommBuffer *b, MyPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MyPacket& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:34</tt> by nedtool.
 * <pre>
 * packet MPCP extends MyPacket
 * {
 *     uint16_t opcode;
 *     uint32_t ts;
 * }
 * </pre>
 */
class MPCP : public ::MyPacket
{
  protected:
    uint16_t opcode_var;
    uint32_t ts_var;

  private:
    void copy(const MPCP& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MPCP&);

  public:
    MPCP(const char *name=NULL, int kind=0);
    MPCP(const MPCP& other);
    virtual ~MPCP();
    MPCP& operator=(const MPCP& other);
    virtual MPCP *dup() const {return new MPCP(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual uint16_t getOpcode() const;
    virtual void setOpcode(uint16_t opcode);
    virtual uint32_t getTs() const;
    virtual void setTs(uint32_t ts);
};

inline void doPacking(cCommBuffer *b, MPCP& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MPCP& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:40</tt> by nedtool.
 * <pre>
 * packet RTTReg extends MPCP
 * {
 *     simtime_t Rtt;
 *     //    int ByteLength; // eth_header(14) + payload(1500) + FCS(4) = 1518
 *     bool energySaving;
 * }
 * </pre>
 */
class RTTReg : public ::MPCP
{
  protected:
    simtime_t Rtt_var;
    bool energySaving_var;

  private:
    void copy(const RTTReg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RTTReg&);

  public:
    RTTReg(const char *name=NULL, int kind=0);
    RTTReg(const RTTReg& other);
    virtual ~RTTReg();
    RTTReg& operator=(const RTTReg& other);
    virtual RTTReg *dup() const {return new RTTReg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual simtime_t getRtt() const;
    virtual void setRtt(simtime_t Rtt);
    virtual bool getEnergySaving() const;
    virtual void setEnergySaving(bool energySaving);
};

inline void doPacking(cCommBuffer *b, RTTReg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, RTTReg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:47</tt> by nedtool.
 * <pre>
 * packet MPCPAutoDiscovery extends MPCP
 * {
 *     // use for rtt test
 *     simtime_t rtt;
 * }
 * </pre>
 */
class MPCPAutoDiscovery : public ::MPCP
{
  protected:
    simtime_t rtt_var;

  private:
    void copy(const MPCPAutoDiscovery& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MPCPAutoDiscovery&);

  public:
    MPCPAutoDiscovery(const char *name=NULL, int kind=0);
    MPCPAutoDiscovery(const MPCPAutoDiscovery& other);
    virtual ~MPCPAutoDiscovery();
    MPCPAutoDiscovery& operator=(const MPCPAutoDiscovery& other);
    virtual MPCPAutoDiscovery *dup() const {return new MPCPAutoDiscovery(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual simtime_t getRtt() const;
    virtual void setRtt(simtime_t rtt);
};

inline void doPacking(cCommBuffer *b, MPCPAutoDiscovery& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MPCPAutoDiscovery& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:53</tt> by nedtool.
 * <pre>
 * packet MPCPGate extends MPCP
 * {
 *     simtime_t StartTime;
 *     uint16_t Length;
 *     uint32_t downLength;
 *     uint32_t queueLength[2];
 * 
 *     // version 0
 *     //    simtime_t powerSavingStartTime;
 *     simtime_t powerSavingTime;
 *     uint16_t powerSavingMode;
 * 
 *     // version 1
 *     simtime_t pwsTime[2];
 *     uint16_t pwsMode[2];
 * 
 *     // loading aware
 *     double downQueueLoading;
 * 
 * }
 * </pre>
 */
class MPCPGate : public ::MPCP
{
  protected:
    simtime_t StartTime_var;
    uint32_t Length_var;
    uint32_t downLength_var;
    uint32_t queueLength_var[2];
    simtime_t powerSavingTime_var;
    uint16_t transmitChannel ;
    uint16_t powerSavingMode_var;
    simtime_t pwsTime_var[2];
    uint16_t pwsMode_var[2];
    double downQueueLoading_var;

  private:
    void copy(const MPCPGate& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MPCPGate&);

  public:
    MPCPGate(const char *name=NULL, int kind=0);
    MPCPGate(const MPCPGate& other);
    virtual ~MPCPGate();
    MPCPGate& operator=(const MPCPGate& other);
    virtual MPCPGate *dup() const {return new MPCPGate(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual simtime_t getStartTime() const;
    virtual void setStartTime(simtime_t StartTime);
    virtual uint32_t getLength() const;
    virtual void setLength(uint32_t Length);
    virtual uint32_t getDownLength() const;
    virtual void setDownLength(uint32_t downLength);
    virtual unsigned int getQueueLengthArraySize() const;
    virtual uint32_t getQueueLength(unsigned int k) const;
    virtual void setQueueLength(unsigned int k, uint32_t queueLength);
    virtual simtime_t getPowerSavingTime() const;
    virtual void setPowerSavingTime(simtime_t powerSavingTime);
    virtual uint16_t getPowerSavingMode() const;
    virtual void setPowerSavingMode(uint16_t powerSavingMode);
    virtual unsigned int getPwsTimeArraySize() const;
    virtual simtime_t getPwsTime(unsigned int k) const;
    virtual void setPwsTime(unsigned int k, simtime_t pwsTime);
    virtual unsigned int getPwsModeArraySize() const;
    virtual uint16_t getPwsMode(unsigned int k) const;
    virtual void setPwsMode(unsigned int k, uint16_t pwsMode);
    virtual double getDownQueueLoading() const;
    virtual void setDownQueueLoading(double downQueueLoading);
    virtual uint16_t getTransmitChannel() const;
    virtual void setTransmitChannel(uint16_t channel);
};

inline void doPacking(cCommBuffer *b, MPCPGate& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MPCPGate& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>traffic_gen/Messages/EPON_messages.msg:74</tt> by nedtool.
 * <pre>
 * packet MPCPReport extends MPCP
 * {
 *     uint32_t QInfo;
 *     simtime_t powerSavingStartTime;
 *     bool sleepAck = false;
 *     bool dozeAck = false;
 *     bool invisible = false;
 *     uint32_t FinalGrantLen;
 * }
 * </pre>
 */
class MPCPReport : public ::MPCP
{
  protected:
    uint32_t QInfo_var;
    simtime_t powerSavingStartTime_var;
    bool sleepAck_var;
    bool dozeAck_var;
    bool invisible_var;
    uint32_t FinalGrantLen_var;
    uint32_t request_var ;

  private:
    void copy(const MPCPReport& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MPCPReport&);

  public:
    MPCPReport(const char *name=NULL, int kind=0);
    MPCPReport(const MPCPReport& other);
    virtual ~MPCPReport();
    MPCPReport& operator=(const MPCPReport& other);
    virtual MPCPReport *dup() const {return new MPCPReport(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual uint32_t getQInfo() const;
    virtual void setQInfo(uint32_t QInfo);
    virtual simtime_t getPowerSavingStartTime() const;
    virtual void setPowerSavingStartTime(simtime_t powerSavingStartTime);
    virtual bool getSleepAck() const;
    virtual void setSleepAck(bool sleepAck);
    virtual bool getDozeAck() const;
    virtual void setDozeAck(bool dozeAck);
    virtual bool getInvisible() const;
    virtual void setInvisible(bool invisible);
    virtual uint32_t getFinalGrantLen() const;
    virtual void setFinalGrantLen(uint32_t FinalGrantLen);
    virtual void setRequestLen( uint32_t request ) ;
    virtual uint32_t getRequestLen() ;
};

inline void doPacking(cCommBuffer *b, MPCPReport& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MPCPReport& obj) {obj.parsimUnpack(b);}


#endif // ifndef _EPON_MESSAGES_M_H_
