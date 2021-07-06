#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <list>

using namespace std;

struct address {
    string addr;
    long long int amount;

    bool operator<(const address& a2) const {
        return addr > a2.addr;
    }
};

enum tx_type {simple, separable, ambigiuous, intractable, unknown};

struct transaction {
    string tx_hash;
    vector<address> inputs;
    vector<address> outputs;
    long long int fee;
    tx_type type;
};

vector<address> remove_repeats(vector<address> addr_list) {
    sort(addr_list.begin(), addr_list.end());
    vector<address> output;
    for(int i = 1; i < addr_list.size(); i++) {
        if(addr_list[i].addr.compare(addr_list[i-1].addr) == 0) {
            address new_address;
            new_address.addr = addr_list[i].addr;
            new_address.amount = addr_list[i].amount + addr_list[i-1].amount;
            output.push_back(new_address);
        }
        else {
            output.push_back(addr_list[i-1]);
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

bool sort_by_amount(const address& a1, const address& a2) {
    return a1.amount > a2.amount;
}

int binary_search_find_index(vector<address> v, int l, int r, int data) {
   int start = l;
   int end = r;
   if(start == end) {
       if(v[start].amount == data) {
           return start;
       }
       else {
           return -1;
       }
   }
   else {
       int center = (start + end)/2;
       if(data <= v[center].amount) {
           return binary_search_find_index(v, start, center, data);
       }
       else {
           return binary_search_find_index(v, center+1, end, data);
       }
   }
}

// pair<int, long long int> find_closest(vector<address> v, int n, int target) {
//    if (target <= v[0].amount)
//         return pair<int, long long int>(0, v[0].amount);
//     if (target >= v[n - 1].amount)
//         return pair<int, long long int>(0, v[n - 1].amount);
 
//     // Doing binary search
//     int i = 0, j = n, mid = 0;
//     while (i < j) {
//         mid = (i + j) / 2;
 
//         if (v[mid].amount == target)
//             return pair<int, long long int>(mid, v[mid].amount);
 
//         /* If target is less than array element,
//             then search in left */
//         if (target < v[mid].amount) {

//             // If target is greater than previous
//             // to mid, return closest of two
//             if (mid > 0 && target > v[mid - 1].amount)
//                 return pair<int, long long int>(mid-1, get_closest(v[mid - 1].amount,
//                                   v[mid].amount, target));
 
//             /* Repeat for left half */
//             j = mid;
//         }
 
//         // If target is greater than mid
//         else {
//             if (mid < n - 1 && target < v[mid + 1].amount)
//                 return pair<int, long long int>(mid+1, get_closest(v[mid + 1].amount,
//                                   v[mid].amount, target));
//             // update i
//             i = mid + 1;
//         }
//     }
// }

long long int get_closest(long long int val1, long long int val2,
               long long int target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

long long int sum_amounts(vector<address> list) {
    long long int total = 0;
    for(address item: list) {
        total += item.amount;
    }
    return total;
}

vector<address> find_subset(vector<address> values, long long int sum, int n) {

    long long int i, j;
    vector<pair<bool, vector<address>>> can_be_summed;

    for(i = 0; i < sum; i++) {
        can_be_summed.push_back(pair<bool, vector<address>>(0,vector<address>()));
    }

    can_be_summed[0].first = 1;

    for(i = 0; i < n; i++) {
        for(j = sum; j >= values[i].amount; j--) {
            if(can_be_summed[j - values[i].amount].first == 1) {
                can_be_summed[j].first = 1;
                can_be_summed[j].second.push_back(values[i]);
            }
        }
    }
    return can_be_summed[sum].second;
}

tx_type check_for_ambiguity(transaction tx) {

    bool compared = false;
     for(int i = 1; i <tx.inputs.size(); i++) {
            if(tx.inputs[i].amount == tx.inputs[i-1].amount && !compared) {
                if(int j = binary_search_find_index(tx.outputs, 0, tx.outputs.size()-1, tx.inputs[i].amount) > -1) {
                    if(j > 0 && (tx.outputs[j-1].amount == tx.outputs[j].amount)) {
                        return ambigiuous;
                        break;
                    }
                    else if(j < tx.outputs.size() && (tx.outputs[j+1].amount == tx.outputs[j].amount)) {
                        return ambigiuous;
                        break;
                    }
                    else {
                        compared = true;
                        continue;
                    }
                }
                else {
                    compared = true;
                    continue;
                }
            }
            else if(compared == true && tx.inputs[i].amount != tx.inputs[i-1].amount) {
                compared = false;
            }
        }
    return unknown;
}

int main() {
    string path = "C:\\Users\\heavy\\Documents\\Summer2021REU\\2021SummerREUExperiments\\2021SummerREUExperiments\\transactions2.csv";
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
                
                for(int i = 0; i < in_addrs.size(); i++) {
                    address input;
                    input.addr = in_addrs[i];
                    input.amount = in_amounts[i];
                    tx.inputs.push_back(input);
                }

                for(int i = 0; i < out_addrs.size(); i++) {
                    address output;
                    output.addr = out_addrs[i];
                    output.amount = out_amounts[i];
                    tx.outputs.push_back(output);
                }
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
        exit(1);
    }
    for(transaction tx: txs) {

        tx.inputs = remove_repeats(tx.inputs);
        tx.outputs = remove_repeats(tx.outputs);

        for(int i = 0; i < tx.inputs.size(); i++) {
            if(tx.inputs[i].amount <= tx.fee) {
                tx.type = ambigiuous;
                break;
            }
        }
        if(tx.type != unknown) {
            continue;
        }
        
        sort(tx.inputs.begin(), tx.inputs.end(), sort_by_amount);
        sort(tx.outputs.begin(), tx.outputs.end(), sort_by_amount);

    }

    for(transaction tx: txs) {
        if(tx.type != unknown) {
            continue;
        }

        tx.type = check_for_ambiguity(tx);

        bool compared = false;

       
    }

    for(transaction tx: txs) {
        if(tx.type != unknown || tx.inputs.size() + tx.outputs.size() > 20) {
            continue;
        }

        long long int in_total = sum_amounts(tx.inputs);
        long long int out_total = sum_amounts(tx.outputs) + tx.fee;  
        int num_partitions = 0; 
        bool use_inputs;

        address fee;
        fee.addr = "fee";
        fee.amount = tx.fee;

        tx.outputs.push_back(fee);
        sort(tx.outputs.begin(), tx.outputs.end(), sort_by_amount);
        sort(tx.inputs.begin(), tx.inputs.end(), sort_by_amount);
        tx.fee = 0;

        vector<transaction> partitions;

        vector<address> outputs = tx.outputs;
        vector<address> inputs = tx.inputs;
        address current_node;
        vector<address> list_to_pull;

        while(inputs.size() > 0 && outputs.size() > 0) {
            if(outputs[0].amount > inputs[0].amount) {
            current_node = outputs[0];
            list_to_pull = inputs;
            use_inputs = false;
            }
            else {
                current_node = inputs[0];
                list_to_pull = outputs;
                use_inputs = true;
            }

            transaction partition;
            partition.tx_hash = tx.tx_hash + "p";
            partition.fee = 0;

            for(int i = list_to_pull.size()-1; i >0; i--) {
                if(!use_inputs) {
                    vector<address> result = find_subset(inputs, current_node.amount, i);
                    if(result.size() > 0) {
                        vector<address> p_ins;
                        vector<address> p_outs;
                        vector<address> temp;
                        p_ins.push_back(current_node);
                        inputs.erase(inputs.begin());
                        p_outs = result;
                        set_difference(outputs.begin(), outputs.end(), result.begin(), result.end(), back_inserter(temp),
                        [](auto &a, auto &b) {
                            return a.amount == b.amount;
                        });

                        outputs = temp;
                        partition.inputs = p_ins;
                        partition.outputs = p_outs;
                        partitions.push_back(partition);
                        in_total -= current_node.amount;
                        out_total -= current_node.amount;;
                        break;
                    }
                } else {
                    vector<address> result = find_subset(outputs, current_node.amount, i);
                    if(result.size() > 0) {
                        vector<address> p_ins;
                        vector<address> p_outs;
                        vector<address> temp;
                        p_outs.push_back(current_node);
                        outputs.erase(outputs.begin());
                        p_ins = result;
                        set_difference(inputs.begin(), inputs.end(), result.begin(), result.end(), back_inserter(temp),
                        [](auto &a, auto &b) {
                            return a.amount == b.amount;
                        });

                        inputs = temp;
                        partition.inputs = p_ins;
                        partition.outputs = p_outs;
                        partitions.push_back(partition);
                        in_total -= current_node.amount;
                        out_total -= current_node.amount;
                        break;
                }
            
            }
        }
        if(partitions.size() == 0) {
            tx.type = simple;
            break;
        }

        transaction simplified_tx;
        simplified_tx.tx_hash = tx.tx_hash;
        simplified_tx.fee = 0;
        simplified_tx.type = unknown;

        int p_num = 0;
        for(transaction p: partitions) {
            address shrunk_input;
            address shrunk_output;

            shrunk_input.addr = "i" + p_num;
            for(int i = 0; i < p.inputs.size(); i++) {
                shrunk_input.amount += p.inputs[i].amount;
            }
            simplified_tx.inputs.push_back(shrunk_input);

            shrunk_output.addr = "o" + p_num;
            for(int i = 0; i < p.outputs.size(); i++) {
                shrunk_output.amount += p.outputs[i].amount;
            }
            simplified_tx.outputs.push_back(shrunk_output);

            p_num++;
        }

        sort(simplified_tx.inputs.begin(), simplified_tx.inputs.end(), sort_by_amount);
        sort(simplified_tx.outputs.begin(), simplified_tx.outputs.end(), sort_by_amount);

        tx_type type = check_for_ambiguity(simplified_tx);
        if(type == unknown) {
            type = separable;
        }

    }
    }
    cout << "Program finished, exiting." << endl;
}

