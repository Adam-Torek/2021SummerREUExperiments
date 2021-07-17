import networkx as nx
import os, json

g = nx.DiGraph()

path_to_json = 'json_files/'

for file_name in [file for file in os.listdir(path_to_json) if file.endswith('.json')]:
    with open(path_to_json + file_name) as json_file:
        data = json.load(json_file)
        for tx in data["tx"]:
            for i in tx["out"]:
                print(i["hash"])