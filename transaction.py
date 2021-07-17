from csv import DictReader
import csv, sys

class transaction():
    def __init__(self, input_addrs, input_values, output_addrs, output_values, fee) -> None:

        inputs = list()
        outputs = list()

        for i in range(len(output_addrs)):
            outputs.append(self.address(output_addrs[i], output_values[i]))

        for i in range(len(input_addrs)):
            inputs.append(self.address(input_addrs[i], input_values[i]))

        self.inputs = inputs
        self.outputs = outputs
        self.fee = fee

    class address():
        def __init__(self, address, amount) -> None:
            self.address = address
            self.amount = amount

def read_csv_file(path, file_names, include_address_dict):
    field_size_limit = sys.maxsize

    while True:
        try:
            csv.field_size_limit(field_size_limit)
            break
        except OverflowError:
            field_size_limit = int(field_size_limit / 10)

    transactions = {}
    addresses = {}
    file_data = {}
    for file in file_names:
        with open(path + file,'r',newline='\n') as csvfile:
            csv_reader = DictReader(csvfile)
            i = 0
            for row in csv_reader:
                inputs = row['input_addresses'].split(':')
                input_values = row['input_values'].split(':')
                outputs = row['output_addresses'].split(':')
                output_values = row['output_values'].split(':')
                
                if include_address_dict:
                    
                    for a in inputs:
                        if a not in addresses.keys():
                            addresses[a] = list()
                        addresses[a].append(True)
                    for a in outputs:
                        if a not in addresses.keys():
                            addresses[a] = list()
                        addresses[a].append(False)

                transactions[row["transaction_hash"]] = (transaction(inputs, input_values,
                outputs, output_values, row["transaction_fee"]))
                i += 1
        file_data[file.split(".")[0]] = i
    
    csvfile.close()
    return (transactions, addresses, file_data)
        