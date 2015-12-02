#include <iostream>
#include "../TaxiVis/KdTrip.hpp"

using namespace std;

int main(int argc, char** argv){

    if(argc != 2){
	cout << "usage: testQuery <input file>" << endl;
	return 0;
    }

    string fname(argv[1]);
    KdTrip kdtrip(fname);        

    KdTrip::Query query;
    query.setPickupTimeInterval(query.createTime(2011,1,7,12,30,0),
                                query.createTime(2014,1,7,13,10,59));
    query.setDropoffTimeInterval(query.createTime(2011,1,7,12,30,0),
                                 query.createTime(2014,1,7,13,10,59));

    KdTrip::QueryResult result = kdtrip.execute(query);
    cout << "Num Trips " << result.size() << endl;
    KdTrip::QueryResult::iterator it;
    for (it=result.begin(); it<result.end(); ++it) {
        const KdTrip::Trip trip = *it;
        printf("Trip\n");
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
    }

    
}
