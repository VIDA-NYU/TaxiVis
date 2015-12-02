#include <stdio.h>
#include <stdint.h>
#include "../TaxiVis/KdTrip.hpp"

struct Trip {
  uint64_t db_idx; // 8 bytes
  double pick_x, pick_y; //8+8 bytes                                                                                                        
  double drop_x, drop_y; //8+8 bytes
  uint64_t pickup_time; // 8 bytes
  uint64_t dropoff_time; // 8 bytes
  char vendor[4]; // 4 bytes
  uint32_t duration; // 4 bytes
  float miles; // 4 bytes
  uint32_t calc_field;//#calculated field here //4 bytes
  uint16_t fare; // 2 bytes
  uint16_t surcharge; // 2 bytes
  uint16_t mta_tax; // 2 bytes
  uint16_t tip; // 2 bytes
  uint16_t toll; // 2 bytes
  uint16_t total; // 2 bytes
  uint16_t medallion_id; // 2 bytes
  uint16_t license_id; // 2 bytes
  bool store_and_forward; // 1 byte
  uint8_t payment_type; // 2 byte
  uint8_t passengers; // 1 byte
  uint8_t rate_code; // 1 byte
};

// The calc_field use the order below
//0 id, 1 medallion_id, 2 license_id, 3 pick_time, 4 drop_time, 5 vendor, 6 rate_code                                                       
//7 store_fwd, 8 passengers, 9 duration, 10 miles, 11 fare, 12 surcharge, 13 mta_tax                                                        
//14 tip, 15 toll, 16 total, 17 payment_type, 18 19 pick_loc, 20 21 drop_loc
// We want these bits to be mandatory
//  0 id
//  1 medallion_id
//  3 pick_time
//
//  4 drop_time
//
//  9 duration
// 10 miles
// 11 fares
//
// 14 tip
// 15 toll
//
// 16 total
// 18 pick_loc
// 19 pick_loc
//
// 20 drop_loc
// 21 drop_loc
// MASK = (bin)11 1101 1100 1110 0001 1011
// MASK = (hex) 3    d    c    e    1    b
// MASK = 0x3dce1b


int main(int argc, char **argv)
{
  FILE *fi = fopen(argv[1], "rb");
  FILE *fo = fopen(argv[2], "wb");
  int totalCount = 0;
  int validCount = 0;
  Trip t;
  KdTrip::Trip to;
  uint32_t MASK = ~0x3dce1b;
  while (fread(&t, 1, sizeof(t), fi)>0) {
    totalCount++;
    if ((t.calc_field & MASK)==t.calc_field) {
      validCount++;
      to.pickup_time = t.pickup_time;
      to.dropoff_time = t.dropoff_time;
      to.pickup_long = t.pick_x;
      to.pickup_lat = t.pick_y;
      to.dropoff_long = t.drop_x;
      to.dropoff_lat = t.drop_y;
      to.distance = t.miles*100;
      to.fare_amount = t.fare;
      to.surcharge = t.surcharge;
      to.mta_tax = t.mta_tax;
      to.tip_amount = t.tip;
      to.tolls_amount = t.toll;
      to.id_taxi = t.medallion_id;
      to.payment_type = t.payment_type;
      to.passengers = t.passengers;
      fwrite(&to, 1, sizeof(to), fo);
    }
    if (totalCount%1000000==0) {
      fprintf(stderr, "\r%d/%d", validCount, totalCount);
    }
  }
  fprintf(stderr, "\n");
  fclose(fi);
  fclose(fo);
}
