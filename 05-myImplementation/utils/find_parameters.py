import subprocess
import re

def execute_command(s, k, n):
    command = f".\\main.exe -s {s} -k {k} -N {n}"
    result = subprocess.run(command, capture_output=True, text=True, shell=True)
    #print("command: ", command)
    return result.stdout

def parse_output(output):
    mtree_knn = re.search(r"MTree KNN: \[(.*?)\]", output)
    seqsearch_knn = re.search(r"SeqSearch KNN: \[(.*?)\]", output)
    
    if mtree_knn and seqsearch_knn:
        mtree_knn_list = mtree_knn.group(1)
        seqsearch_knn_list = seqsearch_knn.group(1)
        return mtree_knn_list, seqsearch_knn_list
    return None, None

def find_valid_parameters():
    good_parameters = (None, None, None)
    for s in range(1, 301):
        # if s == 81 or s == 18 or s > 180:
        #     pass
        # else:
        #     continue
        print(f"Seed: {s}")
        # if good_parameters[0] is not None:
        #     # If the last good parameter was 30 seeds behind
        #     if s - good_parameters[0] >= 300:
        #         print("No valid parameters found.")
        #         return good_parameters
        for k in range(3, 8):
            n = 128
            while n >= 1:
                output = execute_command(s, k, n)
                mtree_knn, seqsearch_knn = parse_output(output)
                
                
                if mtree_knn and seqsearch_knn and mtree_knn != seqsearch_knn:
                    if n <= 32:
                        findSol = True
                        print(f"Valid parameters found: S={s}, K={k}, N={n}")
                        if good_parameters[0] is None or n < good_parameters[2]:
                            print("*")
                            good_parameters = s, k, n
                else:
                    findSol = False
                    break
                
                if findSol:
                    n -= 1
                else:
                    n //= 2
    if good_parameters[0] is not None:
        print(f"Good parameters found: S={good_parameters[0]}, K={good_parameters[1]}, N={good_parameters[2]}")
        return good_parameters
    else:
        return None, None, None

if __name__ == "__main__":
    find_valid_parameters()