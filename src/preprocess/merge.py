import sys

def validMatch(tripTokens,fareTokens):
    tripIndicesToMatch = [0,1,2,5]
    fareIndicesToMatch = [0,1,2,3]
    
    match = True

    for (tIndex,fIndex) in zip(tripIndicesToMatch,fareIndicesToMatch):
        match = match and (tripTokens[tIndex] == fareTokens[fIndex])

    return match

numArgs = len(sys.argv)

if numArgs != 4:
    print "Usage: python merge.py <trips data> <fare data> <output file>"
    exit(1)

tripFileName = sys.argv[1]
tripsFile    = open(tripFileName)
tripsHeader  = tripsFile.readline()

fareFileName = sys.argv[2]
fareFile     = open(fareFileName)
faresHeader  = fareFile.readline()

outputFileName = sys.argv[3]
outputFile     = open(outputFileName,"w")

#out header
outputFile.write('pickup_time,dropoff_time,pickup_long,pickup_lat,dropoff_long,dropoff_lat,id_taxi,distance,fare_amount,surcharge,mta_tax,tip_amount,tolls_amount,payment_type,passengers,field1,field2,field3,field4\n')

#
count = 1
for (tripLine,fareLine) in zip(tripsFile,fareFile):           
    if count % 100000 == 0:
        print 'Processed %d trips' % (count,)
    tripTokens = [t.strip() for t in tripLine.split(',')]
    fareTokens = [t.strip() for t in fareLine.split(',')]
    assert(validMatch(tripTokens,fareTokens))
    
    line = '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,0,0,0,0\n' % (tripTokens[5] ,\
                                                                         tripTokens[6] ,\
                                                                         tripTokens[10],\
                                                                         tripTokens[11],\
                                                                         tripTokens[12],\
                                                                         tripTokens[13],\
                                                                         tripTokens[0] ,\
                                                                         tripTokens[9] ,\
                                                                         fareTokens[5] ,\
                                                                         fareTokens[6] ,\
                                                                         fareTokens[7] ,\
                                                                         fareTokens[8] ,\
                                                                         fareTokens[9] ,\
                                                                         fareTokens[4] ,\
                                                                         tripTokens[7])

    outputFile.write(line)
    count += 1
    


