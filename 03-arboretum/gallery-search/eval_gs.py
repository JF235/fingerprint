import re
import matplotlib.pyplot as plt

def parse_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    queries = []
    current_query = None

    for line in lines:
        if line.startswith('Query:'):
            if current_query:
                queries.append(current_query)
            current_query = {'query': line.strip(), 'results': []}
        elif re.match(r'^\d+:', line):
            current_query['results'].append(line.strip())

    if current_query:
        queries.append(current_query)

    return queries

def check_success(queries):
    success_count = 0

    for query in queries:
        query_id = query['query'].split('-')[0].split('from ')[1]

        top_results = query['results'][:3]
        top_ids = [result.split('-')[0].split('(')[1] for result in top_results]

        if query_id in top_ids:
            success_count += 1
        else:
            for id in list(range(int(query_id)-5, int(query_id)+6)):
                if str(id) in top_ids:
                    success_count += 1
                    break

    return success_count / len(queries) * 100

def plot_success_rate(success_rate):
    plt.figure(figsize=(6, 4))
    plt.bar(['Success Rate'], [success_rate], color='blue')
    plt.ylabel('Percentage')
    plt.title('Success Rate of Queries')
    plt.ylim(0, 100)
    plt.show()

if __name__ == "__main__":
    file_path = 'full_search_st.txt'
    queries = parse_file(file_path)
    success_rate = check_success(queries)
    plot_success_rate(success_rate)
    print(f'Success Rate: {success_rate:.2f}%')