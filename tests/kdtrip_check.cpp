#include "KdTrip.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

static void check(KdTrip &tree, const KdTrip::Query &query) {
    std::vector<const KdTrip::Trip*> expected;
    for (auto it = tree.begin(); it != tree.end(); it++)
        if (query.isMatched(&*it)) expected.push_back(&*it);
    auto result = tree.execute(query);
    auto actual = *result.trips;
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    if (actual != expected)
        throw std::runtime_error("Indexed results differ from brute-force filtering: " +
                                 std::to_string(actual.size()) + " vs " + std::to_string(expected.size()));
}

int main(int argc, char **argv) {
    try {
        if (argc < 3) throw std::runtime_error("Usage: kdtrip_check FILE COUNT [full]");
        KdTrip tree(argv[1]);
        if (std::string(argv[2]) == "validate") return 0;
        const size_t expectedCount = std::stoull(argv[2]);
        const bool full = argc > 3;
        if (tree.tripCount() != expectedCount) throw std::runtime_error("Incorrect stored trip count");
        size_t count = 0;
        uint32_t minTime = UINT32_MAX, maxTime = 0;
        std::vector<KdTrip::Trip> samples;
        size_t stride = std::max(size_t(1), expectedCount / (full ? 8 : 32));
        for (auto it = tree.begin(); it != tree.end(); it++) {
            if (count % stride == 0 && samples.size() < (full ? 8u : 32u)) samples.push_back(*it);
            ++count;
            minTime = std::min(minTime, it->pickup_time);
            maxTime = std::max(maxTime, it->dropoff_time);
        }
        if (count != expectedCount || minTime != tree.minPickupTime() || maxTime != tree.maxDropoffTime())
            throw std::runtime_error("Iterator or cached statistics mismatch");
        if (!full) check(tree, KdTrip::Query());
        size_t checks = full ? 0 : 1;
        for (const auto &trip : samples) {
            for (unsigned dim = 0; dim < 7; ++dim) {
                KdTrip::Query q;
                // Keep full-month test result sizes bounded while still testing
                // each indexed dimension against an independent linear scan.
                if (full) q.setPickupTimeInterval(trip.pickup_time, uint64_t(trip.pickup_time) + 3600);
                switch (dim) {
                case 0: q.setPickupTimeInterval(trip.pickup_time, trip.pickup_time); break;
                case 1: q.setDropoffTimeInterval(trip.dropoff_time, trip.dropoff_time); break;
                case 2: q.minPickupLong = q.maxPickupLong = trip.pickup_long; break;
                case 3: q.minPickupLat = q.maxPickupLat = trip.pickup_lat; break;
                case 4: q.minDropoffLong = q.maxDropoffLong = trip.dropoff_long; break;
                case 5: q.minDropoffLat = q.maxDropoffLat = trip.dropoff_lat; break;
                case 6: q.setTaxiIdRange(trip.id_taxi, trip.id_taxi); break;
                }
                check(tree, q);
                ++checks;
            }
        }
        std::mt19937 random(2013);
        for (unsigned i = 0; i < (full ? 8u : 100u); ++i) {
            const auto &a = samples[random() % samples.size()];
            const auto &b = samples[random() % samples.size()];
            KdTrip::Query q;
            q.setPickupTimeInterval(std::min(a.pickup_time, b.pickup_time),
                                   full ? uint64_t(std::min(a.pickup_time, b.pickup_time)) + 3600
                                        : std::max(a.pickup_time, b.pickup_time));
            q.setDropoffTimeInterval(std::min(a.dropoff_time, b.dropoff_time), std::max(a.dropoff_time, b.dropoff_time));
            q.setPickupArea(std::min(a.pickup_lat, b.pickup_lat), std::min(a.pickup_long, b.pickup_long),
                            std::max(a.pickup_lat, b.pickup_lat), std::max(a.pickup_long, b.pickup_long));
            q.setDropoffArea(std::min(a.dropoff_lat, b.dropoff_lat), std::min(a.dropoff_long, b.dropoff_long),
                             std::max(a.dropoff_lat, b.dropoff_lat), std::max(a.dropoff_long, b.dropoff_long));
            q.setTaxiIdRange(std::min(a.id_taxi, b.id_taxi), std::max(a.id_taxi, b.id_taxi));
            check(tree, q);
            ++checks;
        }
        KdTrip::Query empty;
        empty.setPickupTimeInterval(2, 1);
        check(tree, empty);
        std::cout << count << " trips; " << checks + 1 << " queries match brute force\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
