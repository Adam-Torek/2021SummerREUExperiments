import urllib.request, urllib.error as err, json
import transaction

transactions = transaction.read_csv_file("transactions.csv")

addresses = {}

for t in transactions.values():
    address_list = t.inputs + t.outputs
    for a in address_list:
        try:
            with urllib.request.urlopen("https://api.blockcypher.com/v1/btc/main/addrs/" + a.address) as url:
                data = json.loads(url.read().decode())
                addresses[a.address] = data["n_tx"]
        except err.HTTPError as e:
            print(e)
            pass
            
        
        


