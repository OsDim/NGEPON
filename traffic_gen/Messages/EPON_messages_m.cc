//
// Generated file, do not edit! Created by nedtool 4.6 from traffic_gen/Messages/EPON_messages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "EPON_messages_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(MyPacket);

MyPacket::MyPacket(const char *name, int kind) : ::cPacket(name,kind)
{
    this->SrcAddr_var = 0;
    this->DestAddr_var = 0;
    this->Priority_var = 0;
    this->txEnd_var = 0;
    this->txStart_var = 0;
    this->lastPkt_var = false;
    this->cycleTag_var = 0;
}

MyPacket::MyPacket(const MyPacket& other) : ::cPacket(other)
{
    copy(other);
}

MyPacket::~MyPacket()
{
}

MyPacket& MyPacket::operator=(const MyPacket& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MyPacket::copy(const MyPacket& other)
{
    this->SrcAddr_var = other.SrcAddr_var;
    this->DestAddr_var = other.DestAddr_var;
    this->Priority_var = other.Priority_var;
    this->txEnd_var = other.txEnd_var;
    this->txStart_var = other.txStart_var;
    this->lastPkt_var = other.lastPkt_var;
    this->cycleTag_var = other.cycleTag_var;
}

void MyPacket::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->SrcAddr_var);
    doPacking(b,this->DestAddr_var);
    doPacking(b,this->Priority_var);
    doPacking(b,this->txEnd_var);
    doPacking(b,this->txStart_var);
    doPacking(b,this->lastPkt_var);
    doPacking(b,this->cycleTag_var);
}

void MyPacket::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->SrcAddr_var);
    doUnpacking(b,this->DestAddr_var);
    doUnpacking(b,this->Priority_var);
    doUnpacking(b,this->txEnd_var);
    doUnpacking(b,this->txStart_var);
    doUnpacking(b,this->lastPkt_var);
    doUnpacking(b,this->cycleTag_var);
}

uint16_t MyPacket::getSrcAddr() const
{
    return SrcAddr_var;
}

void MyPacket::setSrcAddr(uint16_t SrcAddr)
{
    this->SrcAddr_var = SrcAddr;
}

uint16_t MyPacket::getDestAddr() const
{
    return DestAddr_var;
}

void MyPacket::setDestAddr(uint16_t DestAddr)
{
    this->DestAddr_var = DestAddr;
}

uint16_t MyPacket::getPriority() const
{
    return Priority_var;
}

void MyPacket::setPriority(uint16_t Priority)
{
    this->Priority_var = Priority;
}

simtime_t MyPacket::getTxEnd() const
{
    return txEnd_var;
}

void MyPacket::setTxEnd(simtime_t txEnd)
{
    this->txEnd_var = txEnd;
}

simtime_t MyPacket::getTxStart() const
{
    return txStart_var;
}

void MyPacket::setTxStart(simtime_t txStart)
{
    this->txStart_var = txStart;
}

bool MyPacket::getLastPkt() const
{
    return lastPkt_var;
}

void MyPacket::setLastPkt(bool lastPkt)
{
    this->lastPkt_var = lastPkt;
}

uint32_t MyPacket::getCycleTag() const
{
    return cycleTag_var;
}

void MyPacket::setCycleTag(uint32_t cycleTag)
{
    this->cycleTag_var = cycleTag;
}

class MyPacketDescriptor : public cClassDescriptor
{
  public:
    MyPacketDescriptor();
    virtual ~MyPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MyPacketDescriptor);

MyPacketDescriptor::MyPacketDescriptor() : cClassDescriptor("MyPacket", "cPacket")
{
}

MyPacketDescriptor::~MyPacketDescriptor()
{
}

bool MyPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MyPacket *>(obj)!=NULL;
}

const char *MyPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MyPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount(object) : 7;
}

unsigned int MyPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *MyPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "SrcAddr",
        "DestAddr",
        "Priority",
        "txEnd",
        "txStart",
        "lastPkt",
        "cycleTag",
    };
    return (field>=0 && field<7) ? fieldNames[field] : NULL;
}

int MyPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='S' && strcmp(fieldName, "SrcAddr")==0) return base+0;
    if (fieldName[0]=='D' && strcmp(fieldName, "DestAddr")==0) return base+1;
    if (fieldName[0]=='P' && strcmp(fieldName, "Priority")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "txEnd")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "txStart")==0) return base+4;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastPkt")==0) return base+5;
    if (fieldName[0]=='c' && strcmp(fieldName, "cycleTag")==0) return base+6;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MyPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint16_t",
        "uint16_t",
        "simtime_t",
        "simtime_t",
        "bool",
        "uint32_t",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : NULL;
}

const char *MyPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MyPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MyPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSrcAddr());
        case 1: return ulong2string(pp->getDestAddr());
        case 2: return ulong2string(pp->getPriority());
        case 3: return double2string(pp->getTxEnd());
        case 4: return double2string(pp->getTxStart());
        case 5: return bool2string(pp->getLastPkt());
        case 6: return ulong2string(pp->getCycleTag());
        default: return "";
    }
}

bool MyPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcAddr(string2ulong(value)); return true;
        case 1: pp->setDestAddr(string2ulong(value)); return true;
        case 2: pp->setPriority(string2ulong(value)); return true;
        case 3: pp->setTxEnd(string2double(value)); return true;
        case 4: pp->setTxStart(string2double(value)); return true;
        case 5: pp->setLastPkt(string2bool(value)); return true;
        case 6: pp->setCycleTag(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MyPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MyPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(MPCP);

MPCP::MPCP(const char *name, int kind) : ::MyPacket(name,kind)
{
    this->opcode_var = 0;
    this->ts_var = 0;
}

MPCP::MPCP(const MPCP& other) : ::MyPacket(other)
{
    copy(other);
}

MPCP::~MPCP()
{
}

MPCP& MPCP::operator=(const MPCP& other)
{
    if (this==&other) return *this;
    ::MyPacket::operator=(other);
    copy(other);
    return *this;
}

void MPCP::copy(const MPCP& other)
{
    this->opcode_var = other.opcode_var;
    this->ts_var = other.ts_var;
}

void MPCP::parsimPack(cCommBuffer *b)
{
    ::MyPacket::parsimPack(b);
    doPacking(b,this->opcode_var);
    doPacking(b,this->ts_var);
}

void MPCP::parsimUnpack(cCommBuffer *b)
{
    ::MyPacket::parsimUnpack(b);
    doUnpacking(b,this->opcode_var);
    doUnpacking(b,this->ts_var);
}

uint16_t MPCP::getOpcode() const
{
    return opcode_var;
}

void MPCP::setOpcode(uint16_t opcode)
{
    this->opcode_var = opcode;
}

uint32_t MPCP::getTs() const
{
    return ts_var;
}

void MPCP::setTs(uint32_t ts)
{
    this->ts_var = ts;
}

class MPCPDescriptor : public cClassDescriptor
{
  public:
    MPCPDescriptor();
    virtual ~MPCPDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPDescriptor);

MPCPDescriptor::MPCPDescriptor() : cClassDescriptor("MPCP", "MyPacket")
{
}

MPCPDescriptor::~MPCPDescriptor()
{
}

bool MPCPDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCP *>(obj)!=NULL;
}

const char *MPCPDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MPCPDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "opcode",
        "ts",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MPCPDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "opcode")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "ts")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint32_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getOpcode());
        case 1: return ulong2string(pp->getTs());
        default: return "";
    }
}

bool MPCPDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: pp->setOpcode(string2ulong(value)); return true;
        case 1: pp->setTs(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MPCPDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(RTTReg);

RTTReg::RTTReg(const char *name, int kind) : ::MPCP(name,kind)
{
    this->Rtt_var = 0;
    this->energySaving_var = 0;
}

RTTReg::RTTReg(const RTTReg& other) : ::MPCP(other)
{
    copy(other);
}

RTTReg::~RTTReg()
{
}

RTTReg& RTTReg::operator=(const RTTReg& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void RTTReg::copy(const RTTReg& other)
{
    this->Rtt_var = other.Rtt_var;
    this->energySaving_var = other.energySaving_var;
}

void RTTReg::parsimPack(cCommBuffer *b)
{
    ::MPCP::parsimPack(b);
    doPacking(b,this->Rtt_var);
    doPacking(b,this->energySaving_var);
}

void RTTReg::parsimUnpack(cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doUnpacking(b,this->Rtt_var);
    doUnpacking(b,this->energySaving_var);
}

simtime_t RTTReg::getRtt() const
{
    return Rtt_var;
}

void RTTReg::setRtt(simtime_t Rtt)
{
    this->Rtt_var = Rtt;
}

bool RTTReg::getEnergySaving() const
{
    return energySaving_var;
}

void RTTReg::setEnergySaving(bool energySaving)
{
    this->energySaving_var = energySaving;
}

class RTTRegDescriptor : public cClassDescriptor
{
  public:
    RTTRegDescriptor();
    virtual ~RTTRegDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(RTTRegDescriptor);

RTTRegDescriptor::RTTRegDescriptor() : cClassDescriptor("RTTReg", "MPCP")
{
}

RTTRegDescriptor::~RTTRegDescriptor()
{
}

bool RTTRegDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RTTReg *>(obj)!=NULL;
}

const char *RTTRegDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RTTRegDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int RTTRegDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *RTTRegDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "Rtt",
        "energySaving",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int RTTRegDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='R' && strcmp(fieldName, "Rtt")==0) return base+0;
    if (fieldName[0]=='e' && strcmp(fieldName, "energySaving")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RTTRegDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
        "bool",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *RTTRegDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int RTTRegDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RTTRegDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getRtt());
        case 1: return bool2string(pp->getEnergySaving());
        default: return "";
    }
}

bool RTTRegDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        case 0: pp->setRtt(string2double(value)); return true;
        case 1: pp->setEnergySaving(string2bool(value)); return true;
        default: return false;
    }
}

const char *RTTRegDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *RTTRegDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(MPCPAutoDiscovery);

MPCPAutoDiscovery::MPCPAutoDiscovery(const char *name, int kind) : ::MPCP(name,kind)
{
    this->rtt_var = 0;
}

MPCPAutoDiscovery::MPCPAutoDiscovery(const MPCPAutoDiscovery& other) : ::MPCP(other)
{
    copy(other);
}

MPCPAutoDiscovery::~MPCPAutoDiscovery()
{
}

MPCPAutoDiscovery& MPCPAutoDiscovery::operator=(const MPCPAutoDiscovery& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPAutoDiscovery::copy(const MPCPAutoDiscovery& other)
{
    this->rtt_var = other.rtt_var;
}

void MPCPAutoDiscovery::parsimPack(cCommBuffer *b)
{
    ::MPCP::parsimPack(b);
    doPacking(b,this->rtt_var);
}

void MPCPAutoDiscovery::parsimUnpack(cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doUnpacking(b,this->rtt_var);
}

simtime_t MPCPAutoDiscovery::getRtt() const
{
    return rtt_var;
}

void MPCPAutoDiscovery::setRtt(simtime_t rtt)
{
    this->rtt_var = rtt;
}

class MPCPAutoDiscoveryDescriptor : public cClassDescriptor
{
  public:
    MPCPAutoDiscoveryDescriptor();
    virtual ~MPCPAutoDiscoveryDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPAutoDiscoveryDescriptor);

MPCPAutoDiscoveryDescriptor::MPCPAutoDiscoveryDescriptor() : cClassDescriptor("MPCPAutoDiscovery", "MPCP")
{
}

MPCPAutoDiscoveryDescriptor::~MPCPAutoDiscoveryDescriptor()
{
}

bool MPCPAutoDiscoveryDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPAutoDiscovery *>(obj)!=NULL;
}

const char *MPCPAutoDiscoveryDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPAutoDiscoveryDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int MPCPAutoDiscoveryDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *MPCPAutoDiscoveryDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "rtt",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int MPCPAutoDiscoveryDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "rtt")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPAutoDiscoveryDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPAutoDiscoveryDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPAutoDiscoveryDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPAutoDiscoveryDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getRtt());
        default: return "";
    }
}

bool MPCPAutoDiscoveryDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        case 0: pp->setRtt(string2double(value)); return true;
        default: return false;
    }
}

const char *MPCPAutoDiscoveryDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MPCPAutoDiscoveryDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(MPCPGate);

MPCPGate::MPCPGate(const char *name, int kind) : ::MPCP(name,kind)
{
    this->StartTime_var = 0;
    this->Length_var = 0;
    this->downLength_var = 0;
    for (unsigned int i=0; i<2; i++)
        this->queueLength_var[i] = 0;
    this->powerSavingTime_var = 0;
    this->powerSavingMode_var = 0;
    for (unsigned int i=0; i<2; i++)
        this->pwsTime_var[i] = 0;
    for (unsigned int i=0; i<2; i++)
        this->pwsMode_var[i] = 0;
    this->downQueueLoading_var = 0;
}

MPCPGate::MPCPGate(const MPCPGate& other) : ::MPCP(other)
{
    copy(other);
}

MPCPGate::~MPCPGate()
{
}

MPCPGate& MPCPGate::operator=(const MPCPGate& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPGate::copy(const MPCPGate& other)
{
    this->StartTime_var = other.StartTime_var;
    this->Length_var = other.Length_var;
    this->downLength_var = other.downLength_var;
    for (unsigned int i=0; i<2; i++)
        this->queueLength_var[i] = other.queueLength_var[i];
    this->powerSavingTime_var = other.powerSavingTime_var;
    this->powerSavingMode_var = other.powerSavingMode_var;
    for (unsigned int i=0; i<2; i++)
        this->pwsTime_var[i] = other.pwsTime_var[i];
    for (unsigned int i=0; i<2; i++)
        this->pwsMode_var[i] = other.pwsMode_var[i];
    this->downQueueLoading_var = other.downQueueLoading_var;
}

void MPCPGate::parsimPack(cCommBuffer *b)
{
    ::MPCP::parsimPack(b);
    doPacking(b,this->StartTime_var);
    doPacking(b,this->Length_var);
    doPacking(b,this->downLength_var);
    doPacking(b,this->queueLength_var,2);
    doPacking(b,this->powerSavingTime_var);
    doPacking(b,this->powerSavingMode_var);
    doPacking(b,this->pwsTime_var,2);
    doPacking(b,this->pwsMode_var,2);
    doPacking(b,this->downQueueLoading_var);
}

void MPCPGate::parsimUnpack(cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doUnpacking(b,this->StartTime_var);
    doUnpacking(b,this->Length_var);
    doUnpacking(b,this->downLength_var);
    doUnpacking(b,this->queueLength_var,2);
    doUnpacking(b,this->powerSavingTime_var);
    doUnpacking(b,this->powerSavingMode_var);
    doUnpacking(b,this->pwsTime_var,2);
    doUnpacking(b,this->pwsMode_var,2);
    doUnpacking(b,this->downQueueLoading_var);
}

uint16_t MPCPGate::getTransmitChannel() const {
    return transmitChannel ;
}

void MPCPGate::setTransmitChannel(uint16_t channel) {
    this->transmitChannel = channel ;
}

simtime_t MPCPGate::getStartTime() const
{
    return StartTime_var;
}

void MPCPGate::setStartTime(simtime_t StartTime)
{
    this->StartTime_var = StartTime;
}

uint32_t MPCPGate::getLength() const
{
    return Length_var;
}

void MPCPGate::setLength(uint32_t Length)
{
    this->Length_var = Length;
}

uint32_t MPCPGate::getDownLength() const
{
    return downLength_var;
}

void MPCPGate::setDownLength(uint32_t downLength)
{
    this->downLength_var = downLength;
}

unsigned int MPCPGate::getQueueLengthArraySize() const
{
    return 2;
}

uint32_t MPCPGate::getQueueLength(unsigned int k) const
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return queueLength_var[k];
}

void MPCPGate::setQueueLength(unsigned int k, uint32_t queueLength)
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->queueLength_var[k] = queueLength;
}

simtime_t MPCPGate::getPowerSavingTime() const
{
    return powerSavingTime_var;
}

void MPCPGate::setPowerSavingTime(simtime_t powerSavingTime)
{
    this->powerSavingTime_var = powerSavingTime;
}

uint16_t MPCPGate::getPowerSavingMode() const
{
    return powerSavingMode_var;
}

void MPCPGate::setPowerSavingMode(uint16_t powerSavingMode)
{
    this->powerSavingMode_var = powerSavingMode;
}

unsigned int MPCPGate::getPwsTimeArraySize() const
{
    return 2;
}

simtime_t MPCPGate::getPwsTime(unsigned int k) const
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return pwsTime_var[k];
}

void MPCPGate::setPwsTime(unsigned int k, simtime_t pwsTime)
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->pwsTime_var[k] = pwsTime;
}

unsigned int MPCPGate::getPwsModeArraySize() const
{
    return 2;
}

uint16_t MPCPGate::getPwsMode(unsigned int k) const
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return pwsMode_var[k];
}

void MPCPGate::setPwsMode(unsigned int k, uint16_t pwsMode)
{
    if (k>=2) throw cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->pwsMode_var[k] = pwsMode;
}

double MPCPGate::getDownQueueLoading() const
{
    return downQueueLoading_var;
}

void MPCPGate::setDownQueueLoading(double downQueueLoading)
{
    this->downQueueLoading_var = downQueueLoading;
}

class MPCPGateDescriptor : public cClassDescriptor
{
  public:
    MPCPGateDescriptor();
    virtual ~MPCPGateDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPGateDescriptor);

MPCPGateDescriptor::MPCPGateDescriptor() : cClassDescriptor("MPCPGate", "MPCP")
{
}

MPCPGateDescriptor::~MPCPGateDescriptor()
{
}

bool MPCPGateDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPGate *>(obj)!=NULL;
}

const char *MPCPGateDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPGateDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount(object) : 9;
}

unsigned int MPCPGateDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *MPCPGateDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "StartTime",
        "Length",
        "downLength",
        "queueLength",
        "powerSavingTime",
        "powerSavingMode",
        "pwsTime",
        "pwsMode",
        "downQueueLoading",
    };
    return (field>=0 && field<9) ? fieldNames[field] : NULL;
}

int MPCPGateDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='S' && strcmp(fieldName, "StartTime")==0) return base+0;
    if (fieldName[0]=='L' && strcmp(fieldName, "Length")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "downLength")==0) return base+2;
    if (fieldName[0]=='q' && strcmp(fieldName, "queueLength")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "powerSavingTime")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "powerSavingMode")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "pwsTime")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "pwsMode")==0) return base+7;
    if (fieldName[0]=='d' && strcmp(fieldName, "downQueueLoading")==0) return base+8;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPGateDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
        "uint16_t",
        "uint32_t",
        "uint32_t",
        "simtime_t",
        "uint16_t",
        "simtime_t",
        "uint16_t",
        "double",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPGateDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPGateDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 3: return 2;
        case 6: return 2;
        case 7: return 2;
        default: return 0;
    }
}

std::string MPCPGateDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getStartTime());
        case 1: return ulong2string(pp->getLength());
        case 2: return ulong2string(pp->getDownLength());
        case 3: return ulong2string(pp->getQueueLength(i));
        case 4: return double2string(pp->getPowerSavingTime());
        case 5: return ulong2string(pp->getPowerSavingMode());
        case 6: return double2string(pp->getPwsTime(i));
        case 7: return ulong2string(pp->getPwsMode(i));
        case 8: return double2string(pp->getDownQueueLoading());
        default: return "";
    }
}

bool MPCPGateDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: pp->setStartTime(string2double(value)); return true;
        case 1: pp->setLength(string2ulong(value)); return true;
        case 2: pp->setDownLength(string2ulong(value)); return true;
        case 3: pp->setQueueLength(i,string2ulong(value)); return true;
        case 4: pp->setPowerSavingTime(string2double(value)); return true;
        case 5: pp->setPowerSavingMode(string2ulong(value)); return true;
        case 6: pp->setPwsTime(i,string2double(value)); return true;
        case 7: pp->setPwsMode(i,string2ulong(value)); return true;
        case 8: pp->setDownQueueLoading(string2double(value)); return true;
        default: return false;
    }
}

const char *MPCPGateDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MPCPGateDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(MPCPReport);

MPCPReport::MPCPReport(const char *name, int kind) : ::MPCP(name,kind)
{
    this->QInfo_var = 0;
    this->powerSavingStartTime_var = 0;
    this->sleepAck_var = false;
    this->dozeAck_var = false;
    this->invisible_var = false;
    this->FinalGrantLen_var = 0;
}

MPCPReport::MPCPReport(const MPCPReport& other) : ::MPCP(other)
{
    copy(other);
}

MPCPReport::~MPCPReport()
{
}

MPCPReport& MPCPReport::operator=(const MPCPReport& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPReport::copy(const MPCPReport& other)
{
    this->QInfo_var = other.QInfo_var;
    this->powerSavingStartTime_var = other.powerSavingStartTime_var;
    this->sleepAck_var = other.sleepAck_var;
    this->dozeAck_var = other.dozeAck_var;
    this->invisible_var = other.invisible_var;
    this->FinalGrantLen_var = other.FinalGrantLen_var;
}

void MPCPReport::parsimPack(cCommBuffer *b)
{
    ::MPCP::parsimPack(b);
    doPacking(b,this->QInfo_var);
    doPacking(b,this->powerSavingStartTime_var);
    doPacking(b,this->sleepAck_var);
    doPacking(b,this->dozeAck_var);
    doPacking(b,this->invisible_var);
    doPacking(b,this->FinalGrantLen_var);
}

void MPCPReport::parsimUnpack(cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doUnpacking(b,this->QInfo_var);
    doUnpacking(b,this->powerSavingStartTime_var);
    doUnpacking(b,this->sleepAck_var);
    doUnpacking(b,this->dozeAck_var);
    doUnpacking(b,this->invisible_var);
    doUnpacking(b,this->FinalGrantLen_var);
}

uint32_t MPCPReport::getQInfo() const
{
    return QInfo_var;
}

void MPCPReport::setQInfo(uint32_t QInfo)
{
    this->QInfo_var = QInfo;
}

simtime_t MPCPReport::getPowerSavingStartTime() const
{
    return powerSavingStartTime_var;
}

void MPCPReport::setPowerSavingStartTime(simtime_t powerSavingStartTime)
{
    this->powerSavingStartTime_var = powerSavingStartTime;
}

bool MPCPReport::getSleepAck() const
{
    return sleepAck_var;
}

void MPCPReport::setSleepAck(bool sleepAck)
{
    this->sleepAck_var = sleepAck;
}

bool MPCPReport::getDozeAck() const
{
    return dozeAck_var;
}

void MPCPReport::setDozeAck(bool dozeAck)
{
    this->dozeAck_var = dozeAck;
}

bool MPCPReport::getInvisible() const
{
    return invisible_var;
}

void MPCPReport::setInvisible(bool invisible)
{
    this->invisible_var = invisible;
}

uint32_t MPCPReport::getFinalGrantLen() const
{
    return FinalGrantLen_var;
}

void MPCPReport::setFinalGrantLen(uint32_t FinalGrantLen)
{
    this->FinalGrantLen_var = FinalGrantLen;
}

void MPCPReport::setRequestLen( uint32_t request ) {
    this->request_var = request ;//new
}

uint32_t  MPCPReport::getRequestLen( ) {
    return request_var ;//new
}

class MPCPReportDescriptor : public cClassDescriptor
{
  public:
    MPCPReportDescriptor();
    virtual ~MPCPReportDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPReportDescriptor);

MPCPReportDescriptor::MPCPReportDescriptor() : cClassDescriptor("MPCPReport", "MPCP")
{
}

MPCPReportDescriptor::~MPCPReportDescriptor()
{
}

bool MPCPReportDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPReport *>(obj)!=NULL;
}

const char *MPCPReportDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPReportDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int MPCPReportDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *MPCPReportDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "QInfo",
        "powerSavingStartTime",
        "sleepAck",
        "dozeAck",
        "invisible",
        "FinalGrantLen",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int MPCPReportDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='Q' && strcmp(fieldName, "QInfo")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "powerSavingStartTime")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sleepAck")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "dozeAck")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "invisible")==0) return base+4;
    if (fieldName[0]=='F' && strcmp(fieldName, "FinalGrantLen")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPReportDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",
        "simtime_t",
        "bool",
        "bool",
        "bool",
        "uint32_t",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPReportDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPReportDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPReportDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getQInfo());
        case 1: return double2string(pp->getPowerSavingStartTime());
        case 2: return bool2string(pp->getSleepAck());
        case 3: return bool2string(pp->getDozeAck());
        case 4: return bool2string(pp->getInvisible());
        case 5: return ulong2string(pp->getFinalGrantLen());
        default: return "";
    }
}

bool MPCPReportDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: pp->setQInfo(string2ulong(value)); return true;
        case 1: pp->setPowerSavingStartTime(string2double(value)); return true;
        case 2: pp->setSleepAck(string2bool(value)); return true;
        case 3: pp->setDozeAck(string2bool(value)); return true;
        case 4: pp->setInvisible(string2bool(value)); return true;
        case 5: pp->setFinalGrantLen(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPReportDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MPCPReportDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


