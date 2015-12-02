#ifndef KD_TRIP_QUERY_HPP
#define KD_TRIP_QUERY_HPP

#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

class KdTrip
{
public:
    struct Trip {
        uint32_t  pickup_time;
        uint32_t  dropoff_time;
        float     pickup_long;
        float     pickup_lat;
        float     dropoff_long;
        float     dropoff_lat;
        //
        uint32_t  field1;
        uint32_t  field2;
        uint32_t  field3;
        uint32_t  field4;
        //
        uint16_t  id_taxi;
        uint16_t  distance;     // in 0.01 miles unit
        uint16_t  fare_amount;  // in cents
        uint16_t  surcharge;    // in cents
        uint16_t  mta_tax;      // in cents
        uint16_t  tip_amount;   // in cents
        uint16_t  tolls_amount; // in cents
        uint8_t   payment_type;
        uint8_t   passengers;
        // bool operator<(const KdTrip::Trip& v) const {//assuming there are no repetitions in the data
        //   return ((pickup_time < v.pickup_time) ||
        //           (pickup_time  == v.pickup_time && dropoff_time < v.dropoff_time) ||
        //           (pickup_time  == v.pickup_time && dropoff_time == v.dropoff_time && id_taxi < v.id_taxi));
        // }
    };
    typedef boost::unordered_set<const Trip*> TripSet;
    // typedef std::set<const Trip*> TripSet;

    struct Query
    {
        Query() {
            minPickupTime = minDropoffTime = 0;
            maxPickupTime = maxDropoffTime = UINT_MAX;
            minTaxiId = 0;
            maxTaxiId = USHRT_MAX;
            minPickupLong = minPickupLat = -FLT_MAX;
            maxPickupLong = maxPickupLat = FLT_MAX;
            minDropoffLong = minDropoffLat = -FLT_MAX;
            maxDropoffLong = maxDropoffLat = FLT_MAX;
        }

        void setPickupTimeInterval(uint64_t t0, uint64_t t1) {
            this->minPickupTime = (uint32_t)t0;
            this->maxPickupTime = (uint32_t)t1;
        }

        void setDropoffTimeInterval(uint64_t t0, uint64_t t1) {
            this->minDropoffTime = (uint32_t)t0;
            this->maxDropoffTime = (uint32_t)t1;
        }

        void setTaxiIdRange(uint16_t id0, uint16_t id1)
        {
            this->minTaxiId = id0;
            this->maxTaxiId = id1;
        }

        void setPickupArea(float lat0, float lon0, float lat1, float lon1)
        {
            this->minPickupLat = lat0;
            this->minPickupLong = lon0;
            this->maxPickupLat = lat1;
            this->maxPickupLong = lon1;
        }

        void setDropoffArea(float lat0, float lon0, float lat1, float lon1)
        {
            this->minDropoffLat = lat0;
            this->minDropoffLong = lon0;
            this->maxDropoffLat = lat1;
            this->maxDropoffLong = lon1;
        }

        bool isMatched(const Trip *trip) const
        {
            return (this->minPickupTime<=trip->pickup_time && trip->pickup_time<=this->maxPickupTime &&
                    this->minDropoffTime<=trip->dropoff_time && trip->dropoff_time<=this->maxDropoffTime &&
                    this->minPickupLong<=trip->pickup_long && trip->pickup_long<=this->maxPickupLong &&
                    this->minPickupLat<=trip->pickup_lat && trip->pickup_lat<=this->maxPickupLat &&
                    this->minDropoffLong<=trip->dropoff_long && trip->dropoff_long<=this->maxDropoffLong &&
                    this->minDropoffLat<=trip->dropoff_lat && trip->dropoff_lat<=this->maxDropoffLat &&
                    this->minTaxiId<=trip->id_taxi && trip->id_taxi<=this->maxTaxiId);
        }

        uint32_t minPickupTime, maxPickupTime;
        uint32_t minDropoffTime, maxDropoffTime;
        uint16_t minTaxiId, maxTaxiId;
        float    minPickupLong, maxPickupLong;
        float    minPickupLat, maxPickupLat;
        float    minDropoffLong, maxDropoffLong;
        float    minDropoffLat, maxDropoffLat;

        inline static uint64_t createTime(int year, int month, int day, int hour, int min, int sec) {
            struct tm timeinfo;
            memset(&timeinfo, 0, sizeof(timeinfo));
            timeinfo.tm_year = year-1900;
            timeinfo.tm_mon  = month-1;
            timeinfo.tm_mday = day;
            timeinfo.tm_hour = hour;
            timeinfo.tm_min  = min;
            timeinfo.tm_sec  = sec;
            timeinfo.tm_isdst = -1;
            return mktime(&timeinfo);
        }
    };

    typedef std::vector<const Trip*> TripVector;

    struct QueryIterator {
        QueryIterator() {}
        QueryIterator(TripVector::const_iterator it): iter(it)  {}
        inline const Trip *  trip() { return *(this->iter); }
        inline const Trip &  operator *() { return *(*this->iter); }
        inline const Trip *  operator->() { return *(this->iter); }
        inline QueryIterator operator++() { this->iter++; return *this; }
        inline QueryIterator operator--() { this->iter--; return *this; }
        inline QueryIterator operator+=(int dif) { this->iter+=dif; return *this; }
        inline QueryIterator operator-=(int dif) { this->iter-=dif; return *this; }
        inline QueryIterator operator++(int) { this->iter++; return *this; }
        inline QueryIterator operator--(int) { this->iter--; return *this; }
        inline ptrdiff_t     operator-(const QueryIterator &it) { return this->iter-it.iter;}
        inline bool          operator==(const QueryIterator &it) const { return this->iter==it.iter; }
        inline bool          operator!=(const QueryIterator &it) const { return this->iter!=it.iter; }
        inline bool          operator<=(const QueryIterator &it) const { return this->iter<=it.iter; }
        inline bool          operator<(const QueryIterator &it) const { return this->iter<it.iter; }
        inline bool          operator>=(const QueryIterator &it) const { return this->iter>=it.iter; }
        inline bool          operator >(const QueryIterator &it) const { return this->iter>it.iter; }
        TripVector::const_iterator iter;
    };

    struct QueryResult {
        typedef QueryIterator iterator;
        inline iterator begin() { return trips.get()?QueryIterator(trips->begin()):QueryIterator(); }
        inline iterator end() { return trips.get()?QueryIterator(trips->end()):QueryIterator(); }
        inline size_t size() { return trips.get()?trips->size():0; }
        boost::shared_ptr<TripVector> trips;
    };

#pragma pack(push, 1)
    struct KdNode {
        uint64_t child_node;
        uint32_t median_value;
    };
#pragma pack(pop)

    struct Iterator {
        Iterator() {}
        Iterator(const Trip*t, const KdNode *e): trip(t), end(e) {}

        const Trip &  operator *() { return *this->trip; }
        const Trip *  operator->() { return this->trip; }
        bool          operator==(const Iterator &it) const { return this->trip==it.trip; }
        bool          operator!=(const Iterator &it) const { return this->trip!=it.trip; }
        Iterator      operator++(int) {
            const KdNode *node = reinterpret_cast<const KdNode*>(this->trip+1);
            while (node<this->end && node->child_node!=0) node++;
            if (node<this->end) {
                this->trip = reinterpret_cast<const Trip*>(&(node->median_value));
            }
            else
                this->trip = reinterpret_cast<const Trip*>(this->end);
            return *this;
        }
    private:
        const Trip *trip;
        const KdNode *end;
    };

public:
    KdTrip(const std::string & treeFileName)
    {
        this->numNodesPerTrip = 1+((sizeof(KdTrip::Trip) + 8)/sizeof(KdNode));
        this->fTree.open(treeFileName);
        this->nodes = reinterpret_cast<const KdNode*>(fTree.data());
        size_t nodeCount = this->fTree.size()/sizeof(KdNode);
        this->endNode = this->nodes+nodeCount;
    }

    Iterator begin()
    {
        const KdNode *node = this->nodes;
        while (node<this->endNode && node->child_node!=0) node++;
        return Iterator(reinterpret_cast<const Trip*>(&(node->median_value)), this->endNode);
    }

    Iterator end()
    {
        return Iterator(reinterpret_cast<const Trip*>(this->endNode), this->endNode);
    }

    QueryResult execute(const Query &q) {
        uint32_t range[7][2] = {
            {q.minPickupTime, q.maxPickupTime},
            {q.minDropoffTime, q.maxDropoffTime},
            {float2uint(q.minPickupLong), float2uint(q.maxPickupLong)},
            {float2uint(q.minPickupLat), float2uint(q.maxPickupLat)},
            {float2uint(q.minDropoffLong), float2uint(q.maxDropoffLong)},
            {float2uint(q.minDropoffLat), float2uint(q.maxDropoffLat)},
            {q.minTaxiId, q.maxTaxiId}
        };
        QueryResult result;
        result.trips = boost::shared_ptr<TripVector>(new TripVector());
        searchKdTree(nodes, 0, range, 0, q, result);
        // std::sort(result.trips->begin(), result.trips->end());
        return result;
    }

    typedef Iterator iterator;
    typedef Iterator const_iterator;

private:
    boost::iostreams::mapped_file_source fTree;
    const KdNode* nodes;
    const KdNode *endNode;
    int     numNodesPerTrip;

    inline bool inRange(uint32_t value, uint32_t range[2]) {
        return (range[0]<=value) && (value<=range[1]);
    }

    void searchKdTree(const KdNode *nodes, uint32_t root, uint32_t range[7][2], int depth, const Query &query, QueryResult &result) {
        const KdNode *node = nodes + root;
        if (node->child_node==-1) return;
        if (node->child_node==0) {
            const Trip *candidate = reinterpret_cast<const Trip*>(&(node->median_value));
            if (query.isMatched(candidate))
                result.trips->push_back(candidate);
            return;
        }
        int rangeIndex = depth%7;
        uint32_t median = node->median_value;
        if (range[rangeIndex][0]<=median)
            searchKdTree(nodes, node->child_node, range, depth+1, query, result);
        if (range[rangeIndex][1]>median) {
            int nextNode = node->child_node+1;
            if (nodes[node->child_node].child_node==0)
                nextNode+=numNodesPerTrip;
            searchKdTree(nodes, nextNode, range, depth+1, query, result);
        }
    }

    uint32_t float2uint(float f) {
        register uint32_t t(*((uint32_t*)&f));
        return t ^ ((-(t >> 31)) | 0x80000000);
    }
};

inline u_int32_t getExtraFieldValue(const KdTrip::Trip* trip,int i){
    switch(i){
    case 0:
        return trip->field1;
        break;
    case 1:
        return trip->field2;
        break;
    case 2:
        return trip->field3;
        break;
    case 3:
        return trip->field4;
        break;
    default:
        assert(false);
    }
}

#endif
