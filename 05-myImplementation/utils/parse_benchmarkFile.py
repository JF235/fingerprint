import re
import json
import argparse
import matplotlib.pyplot as plt

interactive = False

def parse_benchmark_output(output):
    # Regular expressions to extract the relevant information
    dataset_re = re.compile(r"Dataset: (\w+)")
    structure_re = re.compile(r"Structure: (\w+)")
    n_re = re.compile(r"N: (\d+)")
    q_re = re.compile(r"Q: (\d+)")
    k_re = re.compile(r"K: (\d+)")
    s_re = re.compile(r"S: (\d+)")
    m_re = re.compile(r"M: ([\d.]+)")
    d_re = re.compile(r"D: (\d+)")
    ns_re = re.compile(r"NS: (\d+)")
    total_time_re = re.compile(r"Total Time: ([\d.]+) ([mun]?s)")
    average_time_re = re.compile(r"Average Time: ([\d.]+) ([mun]?s)")
    height_re = re.compile(r"Height: (\d+)")
    nodes_accessed_re = re.compile(r"Nodes accessed: (\d+)/(\d+)\(= ([\d.]+)%\)")
    distance_calls_re = re.compile(r"Distance function calls: (\d+)")
    knn_result_re = re.compile(r"KNN\[end\] result: \[(.*)\]")

    # Extract values using regular expressions
    dataset = dataset_re.search(output).group(1)
    structure = structure_re.search(output).group(1)
    n = int(n_re.search(output).group(1))
    q = int(q_re.search(output).group(1))
    k = int(k_re.search(output).group(1))
    s = int(s_re.search(output).group(1))
    m = float(m_re.search(output).group(1))
    d = int(d_re.search(output).group(1))
    ns = int(ns_re.search(output).group(1))
    total_time = float(total_time_re.search(output).group(1))
    average_time = float(average_time_re.search(output).group(1))
    # Turn time into milliseconds if necessary
    time_units = total_time_re.search(output).group(2)
    if time_units == "s":
        total_time *= 1e3
    elif time_units == "ms":
        # Is already in milliseconds
        total_time *= 1
    elif time_units == "us":
        total_time *= 1e-3
    elif time_units == "ns":
        total_time *= 1e-6
    
    time_units = average_time_re.search(output).group(2)
    if time_units == "s":
        average_time *= 1e3
    elif time_units == "ms":
        # Is already in milliseconds
        average_time *= 1
    elif time_units == "us":
        average_time *= 1e-3
    elif time_units == "ns":
        average_time *= 1e-6
    
    
    height_match = height_re.search(output)
    height = int(height_match.group(1)) if height_match else None
    nodes_accessed_match = nodes_accessed_re.search(output)
    nodes_accessed = int(nodes_accessed_match.group(1)) if nodes_accessed_match else None
    total_nodes = int(nodes_accessed_match.group(2)) if nodes_accessed_match else None
    percentage_nodes = float(nodes_accessed_match.group(3)) if nodes_accessed_match else None
    distance_calls = int(distance_calls_re.search(output).group(1))
    knn_result = knn_result_re.search(output).group(1)

    # Parse KNN result
    knn_result = re.findall(r"\(id:(\d+), ([\d.]+)\)", knn_result)
    knn_result = [(int(id_), float(dist)) for id_, dist in knn_result]

    # Create the key and dictionary
    key = f"{dataset}/{structure}/{n}/{q}/{k}/{s}/{m}/{d}/{ns}"
    result_dict = {
        "total_time": total_time,
        "average_time": average_time,
        "height": height,
        "nodes_accessed": nodes_accessed,
        "total_nodes": total_nodes,
        "percentage_nodes": percentage_nodes,
        "distance_calls": distance_calls,
        "result": knn_result
    }

    return key, result_dict

def parse_benchmark_file(file_path):
    with open(file_path, "r") as f:
        content = f.read()

    # Split the content into individual benchmark results
    results = content.split("=" * 100)

    # Parse each result and store in a dictionary
    benchmark_data = {}
    for result in results:
        if result.strip():  # Skip empty results
            key, result_dict = parse_benchmark_output(result)
            benchmark_data[key] = result_dict

    return benchmark_data

def filter_dict(benchmark_data, var, fixed):
    filtered_data = {}
    for key, value in benchmark_data.items():
        key_parts = key.split('/')
        key_dict = {
            "dataset": key_parts[0],
            "structure": key_parts[1],
            "n": key_parts[2],
            "q": key_parts[3],
            "k": key_parts[4],
            "s": key_parts[5],
            "m": key_parts[6],
            "d": key_parts[7],
            "ns": key_parts[8]
        }
        if all(key_dict[k] == str(v) for k, v in fixed.items()):
            filtered_data[key_dict[var]] = value
    return filtered_data

def plot_result(benchmark_data, var, fixed, metric):
    filtered_data = filter_dict(benchmark_data, var, fixed)
    # Check if filtered_data is empty
    if not filtered_data:
        print("No data found for the given parameters.")
        return
    x = sorted(filtered_data.keys(), key=lambda x: float(x))
    y = [filtered_data[k][metric] for k in x]

    plt.figure(figsize=(10, 6))
    plt.plot(x, y, marker='o')
    plt.xlabel(var)
    plt.ylabel(metric)
    plt.title(f"{metric} vs {var}")
    plt.grid(True)
    global interactive
    if interactive:
        plt.show(block=False)
    else:
        plt.show()

def interactive_mode(benchmark_data):
    fixed_params = {
        "dataset": "UNIT",
        "structure": "TREE",
        "n": 100000,
        "q": 500,
        "k": 10,
        "s": 42,
        "m": 10.0,
        "ns": 512
    }
    var = "d"
    metric = "total_time"
    plt.ion()  # Enable interactive mode

    commands = {
        "set fixed": "Set a fixed parameter. Usage: set fixed <param> <value>",
        "reset fixed": "Reset a fixed parameter. Usage: reset fixed <param>",
        "display fixed": "Display fixed parameters. Usage: display fixed <param> or display fixed all",
        "set var": "Set the variable parameter. Usage: set var <param>",
        "set metric": "Set the metric to plot. Usage: set metric <metric>",
        "plot": "Plot the result based on the current settings.",
        "exit": "Exit the interactive mode."
    }

    print("Available commands:")
    for cmd, desc in commands.items():
        print(f"{cmd}: {desc}")

    while True:
        command = input("\nEnter command: ").strip()
        if command.startswith("set fixed"):
            values = command.split()
            # Check if the command is valid
            if len(values) != 4:
                print("Invalid command. Usage: set fixed <param> <value>")
                continue
            else:
                # Unpack
                _, _, param, value = tuple(values)
            try:
                fixed_params[param] = value
            except KeyError:
                print(f"Parameter {param} not found in fixed parameters.")
            print(f"Fixed parameter set: {param} = {value}")
        elif command.startswith("reset fixed"):
            values = command.split()
            if len(values) != 3:
                print("Invalid command. Usage: reset fixed <param>")
                continue
            else:
                # Unpack
                _, _, param = tuple(values)
            if param in fixed_params:
                try:
                    del fixed_params[param]
                    print(f"Fixed parameter {param} reset.")
                except KeyError:
                    print(f"Parameter {param} not found in fixed parameters.")
            else:
                print(f"Parameter {param} not found in fixed parameters.")
        elif command.startswith("display fixed"):
            # Check if command is valid
            values = command.split()
            if len(values) != 3:
                print("Invalid command. Usage: display fixed <param> or display fixed all")
                continue
            else:
                # Unpack
                _, _, param = tuple(values)
            if param == "all":
                print("Fixed parameters:")
                for k, v in fixed_params.items():
                    print(f"{k}: {v}")
            elif param in fixed_params:
                print(f"{param}: {fixed_params[param]}")
            else:
                print(f"Parameter {param} not found in fixed parameters.")
        elif command.startswith("set var"):
            # Check if command is valid
            values = command.split()
            if len(values) != 3:
                print("Invalid command. Usage: set var <param>")
                continue
            else:
                # Unpack
                _, _, var = tuple(values)
            # Check if var is valid
            if var not in ["n", "q", "k", "s", "m", "d", "ns"]:
                print("Invalid variable parameter. Please choose from: n, q, k, s, m, d, ns.")
                var = None
            else:
                print(f"Variable parameter set: {var}")
        elif command.startswith("set metric"):
            # Check if command is valid
            values = command.split()
            if len(values) != 3:
                print("Invalid command. Usage: set metric <metric>")
                continue
            else:
                # Unpack
                _, _, metric = tuple(values)
            # Check if metric is valid
            if metric not in ["total_time", "average_time", "height", "nodes_accessed", "total_nodes", "percentage_nodes", "distance_calls"]:
                print("Invalid metric. Please choose from: total_time, average_time, height, nodes_accessed, total_nodes, percentage_nodes, distance_calls.")
                metric = None
            else:
                print(f"Metric set: {metric}")
        elif command == "plot":
            if var and metric:
                plot_result(benchmark_data, var, fixed_params, metric)
            else:
                print("Please set both variable parameter and result metric before plotting.")
        elif command == "help":
            print("Available commands:")
            for cmd, desc in commands.items():
                print(f"{cmd}: {desc}")
        elif command == "exit":
            print("Exiting interactive mode.")
            break
        else:
            print("Unknown command. Please try again.")

def main():
    parser = argparse.ArgumentParser(description="Parsing the benchmark result.")
    parser.add_argument("-f", "--file", type=str, default="benchmark_results.txt", help="File for parsing")
    parser.add_argument("-i", "--interactive", action="store_true", help="Enable interactive mode")
    args = parser.parse_args()
    file_path = args.file
    
    benchmark_data = parse_benchmark_file(file_path)

    # Default values
    fixed_params = {
        "dataset": "UNIT",
        "structure": "TREE",
        "n": 100000,
        "q": 500,
        "k": 10,
        "s": 42,
        "m": 10.0,
        "ns": 512
    }
    var = "d"
    metric = "total_time"

    if args.interactive:
        # Start interactive mode
        global interactive
        interactive = True
        interactive_mode(benchmark_data)
    else:
        # Plot with default values
        plot_result(benchmark_data, var, fixed_params, metric)

if __name__ == "__main__":
    main()