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
#include "radix.h"

#define xDEBUG

int numNodesPerTrip = 1+((sizeof(KdTrip::Trip) + 8)/sizeof(KdTrip::KdNode));

inline uint32_t float2uint(float f) {
  register uint32_t t(*((uint32_t*)&f));
  return t ^ ((-(t >> 31)) | 0x80000000);
}

inline float uint2float(uint32_t f) {
  register uint32_t u = f ^ (((f >> 31) - 1) | 0x80000000);
  return *((float*)&u);
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

void buildKdTree(KdNode *nodes, uint32_t *tmp, KdTrip::Trip *trips, uint64_t n, int depth, uint64_t thisNode, uint64_t &freeNode) {
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
  size_t medianIndex = n/2-1;
  uint32_t median = getUKey(trips[medianIndex], keyIndex);
  //if (depth!=0) { //if inpute file is sorted by pickup time, don't need to resort all the time.
  if (true) {
    for (size_t i=0; i<n; i++)
      tmp[i] = getUKey(trips[i], keyIndex);
    sortArray(tmp, n);
    median = tmp[n/2-1];
    int64_t l = 0;
    int64_t r = n-1;
    while (l<r) {
      while (l<n && getUKey(trips[l], keyIndex)<=median) l++;
      while (r>=0 && getUKey(trips[r], keyIndex)>median) r--;
      if (l<r)
        SWAP(KdTrip::Trip, trips[l], trips[r]);
    }
    medianIndex = r;
    if (medianIndex==n-1)
      medianIndex = n-2;
  }
  node->median_value = median;
  node->child_node = freeNode;
  freeNode += 2 + ((uint64_t)(medianIndex+1<2))*numNodesPerTrip + ((uint64_t)((n-medianIndex-1<2)&&(n-medianIndex-1>0)))*numNodesPerTrip;
  buildKdTree(nodes, tmp, trips, medianIndex+1, depth+1, node->child_node, freeNode);
  if (medianIndex<n-1)
    buildKdTree(nodes, tmp, trips + medianIndex+1, n-medianIndex-1, depth+1,
                node->child_node+1+((uint64_t)(medianIndex+1<2))*numNodesPerTrip, freeNode);
  else
    nodes[node->child_node+1].child_node = -1;
}

void createKdTree(int argc, char **argv) {
  fprintf(stderr, "Creating KD tree\n");
  boost::iostreams::mapped_file mfile(std::string(argv[1]),
                                      boost::iostreams::mapped_file::priv);
  uint64_t n = mfile.size()/sizeof(KdTrip::Trip);
  KdTrip::Trip *trips = (KdTrip::Trip*)mfile.const_data();
#ifdef DEBUG
  for(uint i = 0 ; i < n ; i++){
      KdTrip::Trip trip = trips[i];
      printf("Trip %d\n",i);
      printf("    Taxi ID: %d\n",trip.id_taxi);
      printf("    pickup_time: %d\n",trip.pickup_time);
      printf("    dropoff_time: %d\n",trip.dropoff_time);
      printf("    pickup lat: %f\n",trip.pickup_lat);
      printf("    pickup lng: %f\n",trip.pickup_long);
      printf("    dropoff lat: %f\n",trip.dropoff_lat);
      printf("    dropoff lng: %f\n",trip.dropoff_long);
      printf("    distance: %d\n",trip.distance);
      printf("    fare: %d\n",trip.fare_amount);
      printf("    passengers: %d\n",trip.passengers);
      printf("    payment type: %d\n",trip.payment_type);
      printf("    field1: %d\n",trip.field1);
      printf("    field2: %d\n",trip.field2);
      printf("    field3: %d\n",trip.field3);
      printf("    field4: %d\n",trip.field4);

      struct tm * timeinfo;
      time_t pickTime = trip.pickup_time;
      timeinfo = localtime (&pickTime);
      printf ("   pickupTime: %s", asctime(timeinfo));
      time_t dropTime = trip.dropoff_time;
      timeinfo = localtime (&dropTime);
      printf ("   dropoffTime: %s", asctime(timeinfo));
  }
#endif

  KdNode *nodes = (KdNode*)malloc(sizeof(KdNode)*((numNodesPerTrip+1)*n+n*3/2));
  uint32_t *tmp = (uint32_t*)malloc(sizeof(uint32_t)*n);
  
  assert(nodes != NULL);
  assert(tmp!= NULL);

  uint64_t freeNode = 1;
  buildKdTree(nodes, tmp, trips, n, 0, 0, freeNode);

  // Writing new indices file
  fprintf(stderr, "\rWriting %llu nodes to %s\n", freeNode, argv[2]);
  FILE *fo = fopen(argv[2], "wb");
  fwrite(nodes, sizeof(KdNode), freeNode, fo);
  fclose(fo);  
  mfile.close();
  free(nodes);
  free(tmp);

}

int main(int argc, char **argv) {
  if (argc!=3) {
    fprintf(stderr, "Usage: %s  <IN_TAXI_TRIP_RECORDS_FILE>  <<OUT_KDTRIP_FILE>\n", argv[0]);
    return -1;
  }  
  createKdTree(argc, argv);
  return 0;
}
