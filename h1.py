from csv import DictReader
import csv
from enum import Enum
import urllib.request, json
import math
    
class transaction():
    def __init__(self, hash, inputs, input_values, outputs, output_values, fee) -> None:
        self.hash = hash
        broke_outputs = outputs.split(':')
        broke_output_vals = output_values.split(':')
        broke_inputs = inputs.split(':')
        broke_input_vals = input_values.split(':')

        inputs = list()
        outputs = list()

        for i in range(len(broke_outputs)):
            outputs.append(self.address(broke_outputs[i], broke_output_vals[i]))

        for i in range(len(broke_inputs)):
            inputs.append(self.address(broke_inputs[i], broke_input_vals[i]))

        self.inputs = inputs
        self.outputs = outputs
        self.fee = fee

    class address():
        def __init__(self, address, amount) -> None:
            self.address = address
            self.amount = amount

class heuristic(Enum):
    h1 = 1
    h2 = 2
    h3 = 3
    h4 = 4

def digits(num):
    return int(math.log10(num))+2

h = heuristic.h2

transactions = list()
with open('transactions.csv','r',newline='\n') as csvfile:
    csv_reader = DictReader(csvfile)
    for row in csv_reader:
        transactions.append(transaction(row['transaction_hash'], row['input_addresses'], row['input_values'],
        row['output_addresses'], row['output_values'], row['transaction_fee']))

c_num = 0
clusters = {}

flagged_addrs = {}

for i in range(len(transactions)):
    if h == heuristic.h1:
        outputs = transactions[i].outputs
        inputs = transactions[i].inputs
        if len(outputs) == 1 and len(inputs) > 1:
            in_addresses = list()
            for i in range(len(inputs)):
                in_addresses.append(inputs[i].address)
            in_addresses.append(outputs[0].address)

            clusters['c' + str(c_num)] = in_addresses
            c_num += 1

    elif h == heuristic.h2:
        if len(transactions[i].outputs) > 2 and len(transactions[i].inputs) > 2:
            list_set = set(transactions[i].outputs)
            check1 = list(list_set)
            check2 = list()
            if len(check1) > 0:
                for a in check1:
                    if a not in transactions[i].inputs:
                        check2.append(a)
                if len(check2) > 0: 
                    flagged_addrs[a.address] = transactions[i].hash
        elif len(transactions[i].outputs) == 2:
            outputs = transactions[i].outputs
            difference = digits(int(outputs[1].amount)) - digits(int(outputs[0].amount))
            if difference > 3:
                flagged_addrs[outputs[1].address] = transactions[i].hash
            elif difference < -3:
                flagged_addrs[outputs[0].address] = transactions[i].hash

if h == heuristic.h2:
    i = 0
    change_addrs = {}
    addr_string = ""
    for a,v in flagged_addrs.items():
        if((i % 50 == 0 or i == len(flagged_addrs)-1) and i != 0):
            addr_string = addr_string[:-1]
            with urllib.request.urlopen("https://blockchain.info/multiaddr?active=" + addr_string) as url:
                data = json.loads(url.read().decode())
                for m in data["addresses"]:
                    if int(m["n_tx"]) <= 2:
                        change_addrs[a] = v
                addr_string = ""
        else:
            addr_string += a + "|"
        i += 1

    for tx in transactions:
        result = [a,v for x in change_addrs.items() if x in tx.inputs or x in tx.outputs]
        if len(result) > 0:
            clusters[c_num].append(result)

    

for t in transactions:
    if len(t.inputs) > 1 and len(t.outputs) > 1:
        address_list = t.inputs
        address_list.append(t.outputs)
        for i in address_list:
            result = [k for k,v in clusters.items() for x in v if x == i.address]
            if len(result) == 1:
                i.address = result[0]
            elif len(result) > 1:
                largest = result[0]
                for j in result:
                    if len(clusters[j]) > len(clusters[largest]):
                        largest = j
                i.address = largest
            else:
                continue


with open('transactions_'+str(h)+'.csv','w') as output:
    wtr = csv.writer(output)
    wtr.csv

