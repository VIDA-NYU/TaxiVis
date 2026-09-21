#!/usr/bin/env julia

using CSV
using DataFrames
using Dates

# Trip struct matching C++ KdTrip::Trip (56 bytes total)
struct Trip
    pickup_time::UInt32
    dropoff_time::UInt32
    pickup_long::Float32
    pickup_lat::Float32
    dropoff_long::Float32
    dropoff_lat::Float32
    field1::UInt32
    field2::UInt32
    field3::UInt32
    field4::UInt32
    id_taxi::UInt16
    distance::UInt16
    fare_amount::UInt16
    surcharge::UInt16
    mta_tax::UInt16
    tip_amount::UInt16
    tolls_amount::UInt16
    payment_type::UInt8
    passengers::UInt8
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
        println("Usage: julia -t auto csv2Binary_mt.jl <input CSV> <output binary file>")
        println("Note: Use -t auto or -t 16 to enable multithreading")
        exit(1)
    end

    input_file = ARGS[1]
    output_file = ARGS[2]

    println("Using $(Threads.nthreads()) threads")
    println()

    println("Loading CSV data from: $input_file")
    flush(stdout)
    df = CSV.read(input_file, DataFrame,
                  stripwhitespace=true,
                  ntasks=Threads.nthreads())

    n_trips = nrow(df)
    println("Loaded $n_trips trips")
    println()

    # Build ID mappings (must be sequential for deterministic output)
    println("Building taxi ID and payment type mappings...")
    taxi_id_map = Dict{String, UInt16}()
    payment_type_map = Dict{String, UInt8}()

    for row in eachrow(df)
        taxi_str = string(row.id_taxi)
        if !haskey(taxi_id_map, taxi_str)
            taxi_id_map[taxi_str] = UInt16(length(taxi_id_map))
        end

        payment_str = string(row.payment_type)
        if !haskey(payment_type_map, payment_str)
            payment_type_map[payment_str] = UInt8(length(payment_type_map))
        end
    end

    println("  Unique taxis: $(length(taxi_id_map))")
    println("  Unique payment types: $(length(payment_type_map))")
    println()

    # Convert to binary in parallel
    println("Converting to binary format (parallel)...")
    flush(stdout)

    trips = Vector{Trip}(undef, n_trips)

    Threads.@threads for i in 1:n_trips
        if i % 200000 == 0
            println("  Processed $i trips")
            flush(stdout)
        end

        row = df[i, :]

        taxi_str = string(row.id_taxi)
        id_taxi = taxi_id_map[taxi_str]

        payment_str = string(row.payment_type)
        payment_type = payment_type_map[payment_str]

        # Handle missing values by replacing with 0
        trips[i] = Trip(
            parse_datetime_to_unix(row.pickup_time),
            parse_datetime_to_unix(row.dropoff_time),
            Float32(coalesce(row.pickup_long, 0.0)),
            Float32(coalesce(row.pickup_lat, 0.0)),
            Float32(coalesce(row.dropoff_long, 0.0)),
            Float32(coalesce(row.dropoff_lat, 0.0)),
            UInt32(coalesce(row.field1, 0)),
            UInt32(coalesce(row.field2, 0)),
            UInt32(coalesce(row.field3, 0)),
            UInt32(coalesce(row.field4, 0)),
            id_taxi,
            UInt16(round(coalesce(row.distance, 0.0) * 100)),
            UInt16(round(coalesce(row.fare_amount, 0.0) * 100)),
            UInt16(round(coalesce(row.surcharge, 0.0) * 100)),
            UInt16(round(coalesce(row.mta_tax, 0.0) * 100)),
            UInt16(round(coalesce(row.tip_amount, 0.0) * 100)),
            UInt16(round(coalesce(row.tolls_amount, 0.0) * 100)),
            payment_type,
            UInt8(coalesce(row.passengers, 0))
        )
    end

    println()
    println("Writing binary output to: $output_file")
    flush(stdout)

    # Write sequentially to maintain order
    open(output_file, "w") do io
        for trip in trips
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

    trip_size = sizeof(Trip)
    total_size = n_trips * trip_size

    println()
    println("Wrote $n_trips trips")
    println("  Size per trip: $trip_size bytes")
    println("  Total size: $total_size bytes ($(round(total_size/1024/1024, digits=1)) MB)")
end

main()
