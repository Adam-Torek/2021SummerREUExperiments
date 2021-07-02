#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

struct address {
    string addr;
    long long int amount;

    bool operator<(const address& a2) const {
        return addr < a2.addr;
    }
};

struct transaction {
    string tx_hash;
    vector<address> input_addrs;
    vector<address> output_addrs;
    long long int fee;
    tx_type type;
};

enum tx_type {simple, separable, ambigiuous, intractable, unknown};

vector<address> remove_repeats(vector<address> addr_list) {
    vector<address> output;
    for(int i = 1; i < addr_list.size(); i++) {
        if(addr_list[i].addr.compare(addr_list[i-1].addr) == 0) {
            address new_address;
            new_address.addr = addr_list[i].addr;
            new_address.amount = addr_list[i].amount + addr_list[i-1].amount;
            output.push_back(new_address);
        }
        else {
            output.push_back(addr_list[i]);
        }
    }
    return output;
}

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

long long int parseInt(const string& input) {
    istringstream converter(input);
    long long int out;
    converter >> out;
    return out;
}

vector<long long int> parseInts(const string& input) {
   vector<long long int> output;
   string value;
   istringstream tokenStream(input);
   while(getline(tokenStream, value, ':'))
   {
      output.push_back(parseInt(value));
   }
    return output;
}

int main() {
    string path = "C:\\Users\\heavy\\Documents\\Summer2021REU\\2021SummerREUExperiments\\2021SummerREUExperiments\\transactions.csv";
    //cout << "Enter a path to a transactions file:" << endl;
    //getline(cin, path);

    //istringstream iss(path);
    //iss >> path;
    fstream transaction_csv;
    vector<transaction> txs;

    string line;
    transaction_csv.open(path);
    if(transaction_csv) {
        cout << "File using path " << path << " was able to be opened." << endl;
        transaction_csv >> line;

        //Loop through each line
        while(!transaction_csv.eof()) {
            transaction_csv >> line;

            vector<string> txString = split(line, ',');
            vector<string> in_addrs = split(txString[1], ':');
            vector<string> out_addrs = split(txString[3],':');
            if(in_addrs.size() > 1 && out_addrs.size() > 1) {
                transaction tx;
                tx.tx_hash = txString[0];
                tx.fee = parseInt(txString[5]);
                tx.type = unknown;

                vector<long long int> in_amounts = parseInts(txString[2]);
                vector<long long int> out_amounts = parseInts(txString[4]); 
                
                vector<address> inputs;
                vector<address> outputs;
                for(int i = 0; i < in_addrs.size(); i++) {
                    address input;
                    input.addr = in_addrs[i];
                    input.amount = in_amounts[i];
                    inputs.push_back(input);
                }

                sort(inputs.begin(), inputs.end());
                inputs = remove_repeats(inputs);

                for(int i = 0; i < inputs.size(); i++) {
                    if(inputs[i].amount <= tx.fee) {
                        tx.fee = tx.fee - inputs[i].amount;
                        inputs.erase(inputs.begin() + i);
                        if(tx.type == unknown) {
                            tx.type == ambigiuous;
                        }
                    }
                }

                for(int i = 0; i < out_addrs.size(); i++) {
                    address output;
                    output.addr = out_addrs[i];
                    output.amount = out_amounts[i];
                    outputs.push_back(output);
                }

                
                sort(outputs.begin(), outputs.end());
                tx.output_addrs = remove_repeats(outputs);
               
                tx.output_addrs = remove_repeats(tx.output_addrs);
                
                txs.push_back(tx);
            }
            else {
                continue;
            }
        }
        transaction_csv.close();
    }
    else {
        cout << "ERROR: File was not found." << endl;
    }
    cout << txs.size() << endl;
}

