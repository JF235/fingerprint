import sys
import os

# Adicione o diretório pai ao sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
import numpy as np
import matplotlib.pyplot as plt

# Dimensions to generate
dimensions = [128, 32, 8, 3]

# Colors for each dimension
colors = plt.cm.viridis(np.linspace(0, 1, len(dimensions)))

N = 100000

# Plot for Euclidean distance
fig1, ax1 = plt.subplots(figsize=(10, 6))
for i, D in enumerate(dimensions):
    unit_vectors = jf.data.generate_unit_vectors(N, D)
    hist_kwargs = {'bins': 30, 'edgecolor': 'black', 'alpha': 0.5, 'color': colors[i], 'label': f'Dimension {D}'}
    jf.plotting.plot_distance_distribution(unit_vectors, jf.math.euclidean_distance, ax=ax1, hist_kwargs=hist_kwargs)

# Customize the plot for Euclidean distance
ax1.set_title('Distance Distribution for Different Dimensions (Euclidean Distance)')
ax1.set_xlabel('Distance')
ax1.set_ylabel('Frequency')
ax1.grid(True)
ax1.legend()

# Plot for Cosine distance
fig2, ax2 = plt.subplots(figsize=(10, 6))
for i, D in enumerate(dimensions):
    unit_vectors = jf.data.generate_unit_vectors(N, D)
    hist_kwargs = {'bins': 30, 'edgecolor': 'black', 'alpha': 0.5, 'color': colors[i], 'label': f'Dimension {D}'}
    jf.plotting.plot_distance_distribution(unit_vectors, jf.math.cosine_distanceU, ax=ax2, hist_kwargs=hist_kwargs)

# Customize the plot for Cosine distance
ax2.set_title('Distance Distribution for Different Dimensions (Cosine Distance)')
ax2.set_xlabel('Distance')
ax2.set_ylabel('Frequency')
ax2.grid(True)
ax2.legend()

# Show the plots
plt.tight_layout()
plt.show()