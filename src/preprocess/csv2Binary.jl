#!/usr/bin/env julia

using CSV
using DataFrames
using Dates

# Trip struct matching C++ KdTrip::Trip (48 bytes total)
# Must match exact layout in KdTrip.hpp
struct Trip
    pickup_time::UInt32      # 4 bytes
    dropoff_time::UInt32     # 4 bytes
    pickup_long::Float32     # 4 bytes
    pickup_lat::Float32      # 4 bytes
    dropoff_long::Float32    # 4 bytes
    dropoff_lat::Float32     # 4 bytes
    field1::UInt32           # 4 bytes
    field2::UInt32           # 4 bytes
    field3::UInt32           # 4 bytes
    field4::UInt32           # 4 bytes
    id_taxi::UInt16          # 2 bytes
    distance::UInt16         # 2 bytes (in 0.01 miles)
    fare_amount::UInt16      # 2 bytes (in cents)
    surcharge::UInt16        # 2 bytes (in cents)
    mta_tax::UInt16          # 2 bytes (in cents)
    tip_amount::UInt16       # 2 bytes (in cents)
    tolls_amount::UInt16     # 2 bytes (in cents)
    payment_type::UInt8      # 1 byte
    passengers::UInt8        # 1 byte
end

# Interpret the CSV wall-clock timestamp in the machine's local time zone,
# matching the original C++ converter (QDateTime::fromString, local time)
# and the queries/display in TaxiVis (mktime / QDateTime::fromTime_t).
# datetime2unix would treat it as UTC and shift every trip by the UTC offset.
function local_unix(dt::DateTime)
    tm = Libc.TmStruct(second(dt), minute(dt), hour(dt), day(dt),
                       month(dt) - 1, year(dt) - 1900, 0, 0, -1)  # isdst=-1: let mktime decide
    return ccall(:mktime, Int, (Ref{Libc.TmStruct},), tm)
end

function parse_datetime_to_unix(dt_str)
    dt = DateTime(dt_str, "yyyy-mm-dd HH:MM:SS")
    return UInt32(local_unix(dt))
end

function main()
    if length(ARGS) != 2
        println("Usage: julia csv2Binary.jl <input CSV> <output binary file>")
        exit(1)
    end

    input_file = ARGS[1]
    output_file = ARGS[2]

    println("Loading CSV data from: $input_file")
    df = CSV.read(input_file, DataFrame, stripwhitespace=true)

    println("Loaded $(nrow(df)) trips")

    # Create mappings for taxi IDs and payment types
    taxi_id_map = Dict{String, UInt16}()
    payment_type_map = Dict{String, UInt8}()

    println("Converting to binary format...")
    open(output_file, "w") do io
        for (i, row) in enumerate(eachrow(df))
            if i % 100000 == 0
                println("  Processed $i trips")
            end

            # Map taxi ID (medallion hash) to integer
            taxi_str = string(row.id_taxi)
            if !haskey(taxi_id_map, taxi_str)
                taxi_id_map[taxi_str] = UInt16(length(taxi_id_map))
            end
            id_taxi = taxi_id_map[taxi_str]

            # Map payment type to integer
            payment_str = string(row.payment_type)
            if !haskey(payment_type_map, payment_str)
                payment_type_map[payment_str] = UInt8(length(payment_type_map))
            end
            payment_type = payment_type_map[payment_str]

            # Create trip struct
            trip = Trip(
                parse_datetime_to_unix(row.pickup_time),
                parse_datetime_to_unix(row.dropoff_time),
                Float32(row.pickup_long),
                Float32(row.pickup_lat),
                Float32(row.dropoff_long),
                Float32(row.dropoff_lat),
                UInt32(row.field1),
                UInt32(row.field2),
                UInt32(row.field3),
                UInt32(row.field4),
                id_taxi,
                UInt16(round(row.distance * 100)),      # Convert to 0.01 miles
                UInt16(round(row.fare_amount * 100)),   # Convert to cents
                UInt16(round(row.surcharge * 100)),     # Convert to cents
                UInt16(round(row.mta_tax * 100)),       # Convert to cents
                UInt16(round(row.tip_amount * 100)),    # Convert to cents
                UInt16(round(row.tolls_amount * 100)),  # Convert to cents
                payment_type,
                UInt8(row.passengers)
            )

            # Write binary struct field by field
            write(io, trip.pickup_time)
            write(io, trip.dropoff_time)
            write(io, trip.pickup_long)
            write(io, trip.pickup_lat)
            write(io, trip.dropoff_long)
            write(io, trip.dropoff_lat)
            write(io, trip.field1)
            write(io, trip.field2)
            write(io, trip.field3)
            write(io, trip.field4)
            write(io, trip.id_taxi)
            write(io, trip.distance)
            write(io, trip.fare_amount)
            write(io, trip.surcharge)
            write(io, trip.mta_tax)
            write(io, trip.tip_amount)
            write(io, trip.tolls_amount)
            write(io, trip.payment_type)
            write(io, trip.passengers)
        end
    end

    trip_count = nrow(df)
    trip_size = sizeof(Trip)
    total_size = trip_count * trip_size

    println("Wrote $trip_count trips")
    println("  Size of trip: $trip_size bytes")
    println("  Total size: $total_size bytes")
    println("  Unique taxis: $(length(taxi_id_map))")
    println("  Unique payment types: $(length(payment_type_map))")
end

main()
