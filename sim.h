struct Packet
{
    int id;
    double request_time;          
    double size;
    float popularity;
};

struct ComparePacketSize {
    bool operator()(Packet const& p1, Packet const& p2)
    {
        return p1.size < p2.size;
    }
};

struct ComparePacketPopularity {
    bool operator()(Packet const& p1, Packet const& p2)
    {
        return p1.popularity < p2.popularity;
    }
};

struct CompareRecentlyRequested {
    bool operator()(Packet const& p1, Packet const& p2)
    {
        return p1.request_time < p2.request_time;
    }
};

struct Event
{
    Event *next;         
    Event *left;         
    Event *right;         
    Event *parent;        
    double key;
    double pack_request_time;
    Packet *packet;                     
    void (*func)(Event*);
    int flag;
};