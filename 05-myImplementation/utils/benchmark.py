import os
import itertools
import subprocess
import argparse

# Define the datasets and structures
#datasets = ["UNIT", "UNIFORM", "FLOAT"]
datasets = ["UNIT"]
structures = ["TREE", "SEQUENTIAL"]

# Define the parameters for the program
N_values = [100000]
Q_values = [500]
K_values = [10]
S_values = [42]
M_values = [10.0]
D_values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 16, 20]
NS_values = [512]

# Output file for benchmark results
output_file = "benchmark_results.txt"

def compile_program(dataset, structure):
    compile_command = f"g++ main.cpp -o main.exe -O3 -D{dataset} -D{structure}"
    print(f"Compiling with dataset={dataset} and structure={structure}")
    subprocess.run(compile_command, shell=True, check=True)

def run_program(N, Q, K, S, M, D, NS, dataset, structure):
    command = f".\\main.exe -N {N} -querySize {Q} -k {K} -s {S} -maxf {M}"
    if dataset in ["UNIT", "UNIFORM"]:
        command += f" -dimension {D}"
    if structure == "TREE":
        command += f" -nodeSize {NS}"
    
    print(f"Running: {command}")
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    
    with open(output_file, "a") as f:
        f.write(f"Dataset: {dataset}, Structure: {structure}, N: {N}, Q: {Q}, K: {K}, S: {S}, M: {M}, D: {D}, NS: {NS}\n")
        f.write(result.stdout)
        f.write(result.stderr)
        f.write("\n")

def main():
    parser = argparse.ArgumentParser(description="Benchmarking script for C++ program.")
    parser.add_argument("-o", "--output", type=str, default="benchmark_results.txt", help="Output file for benchmark results")
    args = parser.parse_args()
    
    global output_file
    output_file = args.output
    
    # Clear the output file
    with open(output_file, "w") as f:
        f.write("Benchmark Results\n")
        f.write("=================\n\n")
    
    # Compile and run benchmarks for each combination of dataset and structure
    for dataset, structure in itertools.product(datasets, structures):
        compile_program(dataset, structure)
        
        # Generate the cartesian product of all parameter values
        for N, Q, K, S, M, D, NS in itertools.product(N_values, Q_values, K_values, S_values, M_values, D_values, NS_values):
            run_program(N, Q, K, S, M, D, NS, dataset, structure)
            with open(output_file, "a") as f:
                f.write("=" * 100 + "\n\n")
            

if __name__ == "__main__":
    main()