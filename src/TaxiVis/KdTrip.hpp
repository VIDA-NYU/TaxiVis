#ifndef KD_TRIP_QUERY_HPP
#define KD_TRIP_QUERY_HPP

#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <vector>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <atomic>
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
    static_assert(sizeof(Trip) == 56, "Unexpected Trip binary layout");
    // Copy-on-write pointer set: taking a worker snapshot is O(1). Iteration
    // is read-only; mutators detach, so GUI edits never race with readers.
    // Optional owners keep mapped or synthetic trip storage alive for jobs.
    class TripSet {
        using Set = boost::unordered_set<const Trip*>;
        std::shared_ptr<Set> data_ = std::make_shared<Set>();
        std::vector<std::shared_ptr<const void>> owners_;
        void detach() { if (!data_.unique()) data_ = std::make_shared<Set>(*data_); }
    public:
        using iterator = Set::const_iterator;
        using const_iterator = Set::const_iterator;
        iterator begin() const { return data_->begin(); }
        iterator end() const { return data_->end(); }
        size_t size() const { return data_->size(); }
        bool empty() const { return data_->empty(); }
        size_t count(const Trip *trip) const { return data_->count(trip); }
        iterator find(const Trip *trip) const { return data_->find(trip); }
        void insert(const Trip *trip) { detach(); data_->insert(trip); }
        template<class It> void insert(It first, It last) { detach(); data_->insert(first, last); }
        void erase(iterator it) { const Trip *trip = *it; detach(); data_->erase(trip); }
        void clear() { data_ = std::make_shared<Set>(); owners_.clear(); }
        void swap(TripSet &other) { data_.swap(other.data_); owners_.swap(other.owners_); }
        void keepAlive(std::shared_ptr<const void> owner) { owners_.push_back(std::move(owner)); }
        void inheritOwners(const TripSet &other) { owners_.insert(owners_.end(), other.owners_.begin(), other.owners_.end()); }
        void unite(const TripSet &other) { insert(other.begin(), other.end()); inheritOwners(other); }
    };
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
    static_assert(sizeof(KdNode) == 12, "Unexpected KdNode binary layout");

    // Number of KdNodes occupied by one leaf: the leaf node plus enough
    // extra nodes to hold the Trip stored starting at its median_value.
    static const int kLeafNodeSpan = 1 + 1 + ((sizeof(Trip) + 8)/sizeof(KdNode));

    struct Iterator {
        Iterator() {}
        Iterator(const Trip*t, const KdNode *e): trip(t), end(e) {}

        const Trip &  operator *() { return *this->trip; }
        const Trip *  operator->() { return this->trip; }
        bool          operator==(const Iterator &it) const { return this->trip==it.trip; }
        bool          operator!=(const Iterator &it) const { return this->trip!=it.trip; }
        Iterator      operator++(int) {
            // A leaf occupies 1+numNodesPerTrip consecutive KdNodes (see
            // build_kdtrip.cpp): the leaf node itself, whose median_value
            // field is the start of the Trip, plus the padding nodes that
            // hold the rest of the Trip. Step past the whole leaf so the
            // scan for the next leaf stays on KdNode boundaries. Stepping
            // from trip+1 instead lands mid-node and reads garbage.
            const KdNode *node = reinterpret_cast<const KdNode*>(
                reinterpret_cast<const char*>(this->trip) - offsetof(KdNode, median_value));
            node += kLeafNodeSpan;
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
        this->fTree.open(treeFileName);
        this->nodes = reinterpret_cast<const KdNode*>(fTree.data());
        size_t nodeCount = this->fTree.size()/sizeof(KdNode);
        if (nodeCount == 0 || fTree.size() % sizeof(KdNode) != 0)
            throw std::runtime_error("Invalid kdtrip file: empty or partial node");
        this->endNode = this->nodes+nodeCount;
        validate(nodeCount);
    }

    Iterator begin()
    {
        const KdNode *node = this->nodes;
        while (node<this->endNode && node->child_node!=0) node++;
        if (node == this->endNode) return end();
        return Iterator(reinterpret_cast<const Trip*>(&(node->median_value)), this->endNode);
    }

    Iterator end()
    {
        return Iterator(reinterpret_cast<const Trip*>(this->endNode), this->endNode);
    }

    size_t tripCount() const { return tripCount_; }
    uint32_t minPickupTime() const { return minPickupTime_; }
    uint32_t maxDropoffTime() const { return maxDropoffTime_; }

    QueryResult execute(const Query &q, const std::atomic_bool *cancel = nullptr) const {
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
        searchKdTree(range, q, result, cancel);
        // std::sort(result.trips->begin(), result.trips->end());
        return result;
    }

    typedef Iterator iterator;
    typedef Iterator const_iterator;

private:
    boost::iostreams::mapped_file_source fTree;
    const KdNode* nodes;
    const KdNode *endNode;

    size_t tripCount_ = 0;
    uint32_t minPickupTime_ = UINT32_MAX;
    uint32_t maxDropoffTime_ = 0;

    // Validate every occupied slot before exposing pointers to callers. The
    // builder allocates children after parents, and each slot belongs to just
    // one node or leaf payload. This rejects cycles, aliases, and stray data.
    void validate(size_t nodeCount) {
        std::vector<bool> occupied(nodeCount, false);
        std::vector<size_t> pending(1, 0);
        size_t occupiedCount = 0;
        while (!pending.empty()) {
            size_t index = pending.back();
            pending.pop_back();
            if (index >= nodeCount || occupied[index])
                throw std::runtime_error("Invalid kdtrip file: overlapping or out-of-range node");
            const KdNode &node = nodes[index];
            size_t span = node.child_node == 0 ? kLeafNodeSpan : 1;
            if (span > nodeCount - index)
                throw std::runtime_error("Invalid kdtrip file: truncated leaf");
            for (size_t i = index; i < index + span; ++i) {
                if (occupied[i])
                    throw std::runtime_error("Invalid kdtrip file: overlapping leaf");
                occupied[i] = true;
                ++occupiedCount;
            }
            if (node.child_node == 0) {
                const Trip *trip = reinterpret_cast<const Trip*>(&node.median_value);
                ++tripCount_;
                minPickupTime_ = std::min(minPickupTime_, trip->pickup_time);
                maxDropoffTime_ = std::max(maxDropoffTime_, trip->dropoff_time);
            } else if (node.child_node != UINT64_MAX) {
                if (node.child_node <= index || node.child_node >= nodeCount)
                    throw std::runtime_error("Invalid kdtrip file: invalid child offset");
                size_t left = static_cast<size_t>(node.child_node);
                size_t leftSpan = nodes[left].child_node == 0 ? kLeafNodeSpan : 1;
                if (leftSpan >= nodeCount - left)
                    throw std::runtime_error("Invalid kdtrip file: missing right child");
                pending.push_back(left + leftSpan);
                pending.push_back(left);
            }
        }
        if (occupiedCount != nodeCount || tripCount_ == 0)
            throw std::runtime_error("Invalid kdtrip file: unreachable data or no trips");
    }

    void searchKdTree(uint32_t range[7][2], const Query &query, QueryResult &result, const std::atomic_bool *cancel) const {
        // Use an explicit stack: legacy indexes with repeated values can be
        // very deep, so recursive traversal can exhaust the process stack.
        typedef std::pair<size_t, unsigned> SearchNode;
        std::vector<SearchNode> pending(1, SearchNode(0, 0));
        size_t visited = 0;
        while (!pending.empty()) {
            if ((++visited % 1024) == 0 && cancel && cancel->load(std::memory_order_relaxed)) break;
            SearchNode current = pending.back();
            pending.pop_back();
            const KdNode *node = nodes + current.first;
            if (node->child_node == UINT64_MAX) continue;
            if (node->child_node == 0) {
                const Trip *candidate = reinterpret_cast<const Trip*>(&node->median_value);
                if (query.isMatched(candidate)) result.trips->push_back(candidate);
                continue;
            }
            unsigned dimension = current.second;
            unsigned nextDimension = (dimension + 1) % 7;
            size_t left = static_cast<size_t>(node->child_node);
            // Existing builders can put values equal to the median on either
            // side. Inclusive bounds must visit both sides at equality.
            if (range[dimension][1] >= node->median_value) {
                size_t right = left + (nodes[left].child_node == 0 ? kLeafNodeSpan : 1);
                pending.push_back(SearchNode(right, nextDimension));
            }
            if (range[dimension][0] <= node->median_value)
                pending.push_back(SearchNode(left, nextDimension));
        }
    }

    static uint32_t float2uint(float f) {
        uint32_t t;
        std::memcpy(&t, &f, sizeof(t));
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
        throw std::out_of_range("Invalid extra field index");
    }
}

#endif
