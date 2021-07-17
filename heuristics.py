from collections import Counter
import csv
from enum import Enum
import math
import transaction
import os
import re
import sys
import getopt

class heuristic(Enum):
    h1 = 1
    h2 = 2
    h3 = 3

def digits(num):
    return int(math.log10(num))+2

def merge_or_create_cluster(source, addresses, c_num):
    intersection = list(set(source) & set(clusters.keys()))

   # Cluster addresses together if any of the source 
   # appear in our clusters
    if intersection:
         for a in addresses:
            clusters[a] = clusters[intersection[0]]
            
    #Else, make a new cluster
    else:
        for a in addresses:
            clusters[a] = 'c' + str(c_num)
        c_num += 1
    
    #return the cluster number
    return c_num

def get_file_names(directory, pattern):
    file_names = []
    for (root, dirs, files) in os.walk(directory):
        for file in files:
            pattern = re.compile(pattern)
            if pattern.match(file):
                file_names.append(file)
    return file_names

#define variables for use
dir = ""
h = heuristic.h1
output_dir = ""

#parse arguments
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:i:o:",["h=","input=","output="])
except getopt.GetoptError:
    print("heuristics.py -h <h1 h2 h3> -i <input dir> -o <output_dir>")
    sys.exit(2)
for opt, arg in opts:
    if opt == "-h":
        h = heuristic.h1

        if arg == "h1":
            h = heuristic.h1
        elif arg == "h2":
            h = heuristic.h2
        elif arg == "h3":
            h = heuristic.h3
        else:
            print("Improper heuristic command entered. Options are h1, h2, h3")
            sys.exit(2)
    elif opt in ("-i","--input"):
        dir = arg
        if not os.path.exists(dir):
            print("Path does not exist. Exiting")
            sys.exit(2)
    elif opt in ("-o", "--output"):
        output_dir = arg

transactions = {}
addresses = {}
file_data = {}

#get ou file names
names = get_file_names(dir, "[0-9]{4}-[0-9]{2}-[0-9]{2}.csv$")

if len(names) == 0:
    print("No properly formatted CSV files found")
    exit(1)

# Pull out all relevant transaction data
result = transaction.read_csv_file(dir, names, h == heuristic.h2)

transactions = result[0]
addresses = result[1]
file_data = result[2]

c_num = 0
clusters = {}

# apply our heuristics depending on which one the user specifies
for k, t in transactions.items():

    outputs = [o.address for o in t.outputs]
    inputs = [i.address for i in t.inputs]
    if h == heuristic.h1:
        if len(outputs) == 1 and len(inputs) > 1:
            address_list = inputs + outputs
            c_num = merge_or_create_cluster(inputs, address_list, c_num)
    elif h == heuristic.h2:

        # case that transaction has only two outputs

        candidates = []
        

        if len(outputs) == 2 and len(inputs) == 1:
            decimals = digits(int(t.outputs[0].amount)) - digits(int(t.outputs[1].amount))
            change_address = None
            if(decimals > 3):
                change_address = outputs[0]
            elif(decimals < -3):
                change_address = outputs[1]
            else:
                continue

            if change_address is not None and len(addresses[change_address]) < 2 and addresses[change_address][0] == False:
                candidates.append(change_address)

        #Else use the normal rules described for H2 to get clusters

        elif len(outputs) > 2 and len(inputs) == 1:
            counts = dict(Counter(outputs))
            candidates = [k for k,v in counts.items() if v <= 1]
            candidates = [x for x in candidates if x not in inputs and len(addresses[x]) <= 2 and addresses[x][0] == False]
            
        #If we have potential change address candidates, run this
        if candidates:
            c_num = merge_or_create_cluster(inputs, candidates, c_num)
   
    elif h == heuristic.h3:
        if len(inputs) == 0:
            c_num = merge_or_create_cluster(outputs, outputs, c_num)

# Apply clusters to many-to-many transactions  
for v in transactions.values():
    if len(v.inputs) > 1 and len(v.outputs) > 1:
        address_list = v.inputs + v.outputs
        for a in address_list:
            if a.address in clusters.keys():
                a.address = clusters[a.address]

#Output data as CSV files
mode = ""
if h == heuristic.h1:
    mode = "h1"
elif h == heuristic.h2:
    mode = "h2"
else:
    mode = "h3"

if(not os.path.exists(output_dir + mode + "/")):
    os.makedirs(output_dir + mode + "/")

for name, l in file_data.items():
    with open(output_dir+ mode + "/" + name + '.csv','w') as output:
        header = ["transaction_hash","input_addresses","input_values","output_addresses","output_values","transaction_fee", "classification"]
        wtr = csv.DictWriter(output, fieldnames=header, lineterminator='\n')
        wtr.writeheader()

        j = 0
        for k, v in transactions.items():
            if(j == l):
                output.close()
                break
            
            inputs = ':'.join([x.address for x in v.inputs])
            input_amounts = ':'.join([str(x.amount) for x in v.inputs])
            outputs =  ':'.join([x.address for x in v.outputs])
            output_amounts = ':'.join([str(x.amount) for x in v.outputs])
            

            wtr.writerow({"transaction_hash": k, "input_addresses": inputs, "input_values" : input_amounts,
            "output_addresses": outputs, "output_values": output_amounts, "transaction_fee" : v.fee, "classification": "unclassified"})
            j += 1
        


