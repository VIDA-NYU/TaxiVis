#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/timer/timer.hpp>
#include "../TaxiVis/KdTrip.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>

#define xDEBUG

int numNodesPerTrip = 1+((sizeof(KdTrip::Trip) + 8)/sizeof(KdTrip::KdNode));

inline uint32_t float2uint(float f) {
  uint32_t t;
  std::memcpy(&t, &f, sizeof(t));
  return t ^ ((-(t >> 31)) | 0x80000000);
}

#pragma pack(push, 1)
struct KdNode {
  uint64_t child_node;
  uint32_t median_value;
};
#pragma pack(pop)

inline uint32_t getUKey(const KdTrip::Trip &trip, int keyIndex)
{
  switch (keyIndex) {
  case 0:
    return trip.pickup_time;
  case 1:
    return trip.dropoff_time;
  case 2:
    return float2uint(trip.pickup_long);
  case 3:
    return float2uint(trip.pickup_lat);
  case 4:
    return float2uint(trip.dropoff_long);
  case 5:
    return float2uint(trip.dropoff_lat);
  case 6:
    return trip.id_taxi;
  default:
    break;
  }
  return 0;
}

void buildKdTree(KdNode *nodes, KdTrip::Trip *trips, uint64_t n, int depth, uint64_t thisNode, uint64_t &freeNode) {
  KdNode *node = nodes + thisNode;
  if (n<2) {
    static uint64_t cnt = 0;
    cnt++;
    if (cnt%500000==0)
      fprintf(stderr, "\r%llu", cnt);
    node->child_node = 0;
    *reinterpret_cast<KdTrip::Trip*>(&(node->median_value)) = *trips;
    return;
  }
  int keyIndex = depth%7;
  // Split by rank, including for identical values. Both subtrees may contain
  // the median, which the inclusive query traversal explicitly supports.
  size_t medianIndex = n/2-1;
  std::nth_element(trips, trips + medianIndex, trips + n,
                   [keyIndex](const KdTrip::Trip &a, const KdTrip::Trip &b) {
                     return getUKey(a, keyIndex) < getUKey(b, keyIndex);
                   });
  uint32_t median = getUKey(trips[medianIndex], keyIndex);
  node->median_value = median;
  node->child_node = freeNode;
  freeNode += 2 + ((uint64_t)(medianIndex+1<2))*numNodesPerTrip + ((uint64_t)((n-medianIndex-1<2)&&(n-medianIndex-1>0)))*numNodesPerTrip;
  buildKdTree(nodes, trips, medianIndex+1, depth+1, node->child_node, freeNode);
  if (medianIndex<n-1)
    buildKdTree(nodes, trips + medianIndex+1, n-medianIndex-1, depth+1,
                node->child_node+1+((uint64_t)(medianIndex+1<2))*numNodesPerTrip, freeNode);
  else
    nodes[node->child_node+1].child_node = -1;
}

void createKdTree(const char *input, const char *output) {
  fprintf(stderr, "Creating KD tree\n");
  if (boost::filesystem::exists(output) && boost::filesystem::equivalent(input, output))
    throw std::runtime_error("Input and output must be different files");
  boost::iostreams::mapped_file mfile(std::string(input), boost::iostreams::mapped_file::priv);
  if (mfile.size() == 0 || mfile.size() % sizeof(KdTrip::Trip) != 0)
    throw std::runtime_error("Input must contain complete 56-byte Trip records");
  size_t n = mfile.size()/sizeof(KdTrip::Trip);
  KdTrip::Trip *trips = reinterpret_cast<KdTrip::Trip*>(mfile.data());
  const size_t slotsPerTrip = KdTrip::kLeafNodeSpan + 1;
  if (n > std::numeric_limits<size_t>::max()/slotsPerTrip/sizeof(KdNode))
    throw std::runtime_error("Input is too large to index");
  // A full binary tree has n leaves and n-1 internal nodes.
  std::vector<KdNode> nodes(slotsPerTrip*n-1);
  uint64_t freeNode = n == 1 ? KdTrip::kLeafNodeSpan : 1;
  buildKdTree(nodes.data(), trips, n, 0, 0, freeNode);

  fprintf(stderr, "Writing %llu nodes to %s\n", static_cast<unsigned long long>(freeNode), output);
  std::ofstream stream(output, std::ios::binary | std::ios::trunc);
  stream.exceptions(std::ios::failbit | std::ios::badbit);
  stream.write(reinterpret_cast<const char*>(nodes.data()), freeNode * sizeof(KdNode));
  stream.close();
}

int main(int argc, char **argv) {
  if (argc!=3) {
    fprintf(stderr, "Usage: %s  <IN_TAXI_TRIP_RECORDS_FILE>  <<OUT_KDTRIP_FILE>\n", argv[0]);
    return -1;
  }  
  try {
    createKdTree(argv[1], argv[2]);
  } catch (const std::exception &error) {
    fprintf(stderr, "Unable to build index: %s\n", error.what());
    return 1;
  }
  return 0;
}
