import re
import matplotlib.pyplot as plt
import numpy as np
import sys

if len(sys.argv) < 2:
    print('Usage: python parse_out.py <filename>')
    sys.exit(1)

filename = sys.argv[1]

# Inicializa listas para armazenar os tempos
total_times = []
avg_times = []
acertos = []
erros = []

# Função para processar cada bloco de consulta
def process_block(block):
    # Extrai o tempo total
    total_time_match = re.search(r'Total Time:\s*([\d\.]+)\s*([um])?s', block)
    if total_time_match:
        # Converte o tempo total para milissegundos caso esteja em segundo
        if total_time_match.group(2) is None:
            total_times.append(float(total_time_match.group(1)) * 1000)
        elif total_time_match.group(2) == 'u':
            total_times.append(float(total_time_match.group(1)) / 1000)
        else:
            total_times.append(float(total_time_match.group(1)))
    
    # Extrai o tempo médio
    avg_time_match = re.search(r'Avg Time:\s*([\d\.]+)\s*([um])s', block)
    if avg_time_match:
        # Converte o tempo médio para microssegundos
        if avg_time_match.group(2) == 'm':
            avg_times.append(float(avg_time_match.group(1)) * 1000)
        else:
            avg_times.append(float(avg_time_match.group(1)))
    
    # Extrai o ID buscado
    query_match = re.search(r'^(\d+)\s*:', block, re.MULTILINE)
    if query_match:
        query_id = query_match.group(1)
    else:
        return
    
    # Extrai a lista de IDs retornados
    ids_match = re.search(r'\[([^\]]+)\]', block)
    if ids_match:
        ids_list = re.findall(r"\'(\w+)\'", ids_match.group(1))
        if query_id in ids_list:
            acertos.append(query_id)
        else:
            erros.append(query_id)

# Leia o conteúdo do arquivo
with open(filename, 'r', encoding='utf-8') as file:
    content = file.read()

# Divida o conteúdo em blocos baseados na ocorrência de 'Query'
blocks = content.split('Query')
for block in blocks[1:]:  # Ignora o primeiro split vazio
    block = 'Query' + block
    process_block(block)

# Plotar os histogramas
plt.figure(figsize=(12, 5))

# Check if total_times shold be adjusted to milliseconds/microseconds/seconds based on the average of each list
if len(total_times) > 0:
    avg_total_times = sum(total_times) / len(total_times)
    if avg_total_times < 1:
        total_times = [t * 1000 for t in total_times]
        total_units = ' (µs)'
    elif avg_total_times > 1000:
        total_times = [t / 1000 for t in total_times]
        total_units = ' (s)'
    else:
        total_units = ' (ms)'

if len(avg_times) > 0:
    avg_avg_times = sum(avg_times) / len(avg_times)
    if avg_avg_times > 1000:
        avg_times = [t / 1000 for t in avg_times]
        avg_units = ' (ms)'
    else:
        avg_units = ' (µs)'

REMOVE_OUTLIERS = True
if REMOVE_OUTLIERS:
    # If is 3x the standard deviation, remove it
    total_times = [t for t in total_times if abs(t - np.mean(total_times)) < 2 * np.std(total_times)]
    avg_times = [t for t in avg_times if abs(t - np.mean(avg_times)) < 2 * np.std(avg_times)]


plt.subplot(1, 2, 1)
plt.hist(total_times, bins=20, color='skyblue', edgecolor='black')
plt.title('Histograma de Tempo Total' + total_units)
plt.xlabel('Tempo Total' + total_units)
plt.ylabel('Frequência')

plt.subplot(1, 2, 2)
plt.hist(avg_times, bins=40, color='salmon', edgecolor='black')
plt.title('Histograma de Tempo Médio' + avg_units)
plt.xlabel('Tempo Médio' + avg_units)
plt.ylabel('Frequência')

plt.tight_layout()
plt.show()

# Calcular e imprimir a porcentagem de acertos
total_queries = len(acertos) + len(erros)
if total_queries > 0:
    porcentagem_acertos = (len(acertos) / total_queries) * 100
    print(f'Porcentagem de acertos: {porcentagem_acertos:.2f}%')
    print(f'Erro: {erros}')
else:
    print('Nenhuma consulta encontrada.')