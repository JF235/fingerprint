import matplotlib.pyplot as plt
from PIL import Image
import numpy as np

datapath = ''

def set_plot_datapath(path):
    global datapath
    datapath = path

def str2FS(image_name: str) -> tuple[int, int]:
    # FS: Finger, Sample
    return tuple(map(int, image_name.strip('A').split('_')))

def FS2str(finger, sample) -> str:
    return f'A{finger}_{sample}'

def get_minutiaXYR(finger, sample, minutia_no) -> tuple[int, int, int]:
    # X, Y coordinates and R rotation angle
    file_path = datapath + f'mnts/A{finger}_{sample}.mnt'
    with open(file_path, 'r') as file:
        lines = file.readlines()

    line = lines[2 + minutia_no]
    x, y, theta, score = map(float, line.split())
    return x, y, theta

def imshow_sample(ax, finger, sample):
    image_path = datapath + f"gabor/bin_enh/A{finger}_{sample}.png"
    image = Image.open(image_path)
    ax.imshow(image, cmap='gray')

def zoom_minutia(ax, finger, sample, minutia_no, x, y):
    patch_path = datapath + f"patches/A{finger}_{sample}_p{minutia_no}.png"
    patch = Image.open(patch_path)
    patch = patch.resize((int(patch.width * .5), int(patch.height * .5)))
    
    region_locator = [x-20, x+20], [y-20, y+20]
    inset_axes_bounds = [x+80, y-100, 80, 80] # x, y, w, h
    axins = ax.inset_axes(inset_axes_bounds, transform=ax.transData, xticklabels=[], yticklabels=[], xlim=region_locator[0], ylim=region_locator[1])
    
    zoomed_bounds = [x-80, x+80, y-80, y+80]
    axins.imshow(patch, extent=zoomed_bounds, cmap='gray')
    for spine in axins.spines.values():
        spine.set_edgecolor('red')
        spine.set_linewidth(2)
    axins.plot(0.5, 0.5, 'rx', transform=axins.transAxes)

    ax.indicate_inset_zoom(axins, alpha=1, lw=2, edgecolor="red")

def mark_minutia(ax, finger, sample, minutia_no, dir_field = False, zoom = False):
    x, y, theta = get_minutiaXYR(finger, sample, minutia_no)
    ax.plot(x, y, 'xr', ms=5)
    if dir_field:
        # Pq sinal negativo?
        ax.plot([x, x + 15*np.cos(theta)], [y, y + 15*np.sin(-theta)], color='r')
    if zoom:
        zoom_minutia(ax, finger, sample, minutia_no, x, y)

def plot_minutiae(finger, sample):   
    # Collect minutiae info in .mnt file
    file_path = datapath + f'mnts/A{finger}_{sample}.mnt'
    with open(file_path, 'r') as file:
        lines = file.readlines()
        
    # --- .MNT FILE ---
    # image_name
    # minutiae_count height width
    # x1 y1 theta1 score1 
    # x2 y2 theta2 score2
    # ...
    image_name = lines[0].strip()
    minutiae_count, height, width = map(int, lines[1].split())

    # Plot image, minutiae position and orientation
    fig, ax = plt.subplots(figsize=(7, 7))
    
    imshow_sample(ax, finger, sample)
    
    for i in range(minutiae_count):
        mark_minutia(ax, finger, sample, i, dir_field=True)

    ax.set_title(f"Minutiae plot for {image_name}: count {minutiae_count}", fontsize = 20)
    ax.axis('off')
    ax.set_xlim([0, width])
    ax.set_ylim([height, 0])
    
def plot_minutia_zoom(finger, sample, minutia_no):
    # Collect minutiae info in .mnt file
    file_path = datapath + f'mnts/A{finger}_{sample}.mnt'
    with open(file_path, 'r') as file:
        lines = file.readlines()
    image_name = lines[0].strip()
    minutiae_count, height, width = map(int, lines[1].split())
    
    # Plot the main image
    fig, ax = plt.subplots(figsize=(7, 7))
    imshow_sample(ax, finger, sample)
    mark_minutia(ax, finger, sample, minutia_no, zoom=True)
    
    ax.set_title(f"Minutiae plot for {image_name}", fontsize=20)
    ax.axis('off')
    ax.set_xlim([0, width])
    ax.set_ylim([height, 0])

def vec2latex(vector, d):
    # Plotando vetor com d componentes
    vector_latex = r"$\begin{bmatrix}"
    for i in range(d):
        vector_latex += f"{vector[i]:.3f} \\\\ "
    vector_latex += r"\vdots\end{bmatrix}_{128\times 1}$"
    return vector_latex

def plot_features(finger: int, sample: int, no_features = 4):
    # Loading features
    filename = datapath + f'features/A{finger}_{sample}.npy'
    features = np.load(filename)

    # Criar subplots
    fig, axs = plt.subplots(nrows=no_features, ncols=2, figsize=(8, 3*no_features))

    # Plotar imagens e vetores
    for i in range(no_features):
    
        # Imagem do patch
        patch_filename = datapath + f'patches/A{finger}_{sample}_p{i}.png'
        img = Image.open(patch_filename)
        axs.flat[2*i].imshow(img, cmap='gray')
        axs.flat[2*i].axis('off')
        
        # Vetor associado ao patch
        v = features[i, :]
        vector_latex = vec2latex(v, 4)
        axs.flat[2*i+1].text(0.5, 0.5, vector_latex, fontsize=24, ha='center', va='center')
        axs.flat[2*i+1].axis('off')
    
    plt.tight_layout()
    
def plot_minutia_compare(samples: list[tuple], minutiae: list[int], zoom=True):
    n = len(minutiae)
    fig, ax = plt.subplots(nrows=1, ncols=n, figsize=(14, 7))
    for i in range(n):
        finger, sample = samples[i]
        # Collect minutiae info
        file_path = datapath + f'mnts/A{finger}_{sample}.mnt'
        with open(file_path, 'r') as file:
            lines = file.readlines()

        image_name = lines[0].strip()
        minutiae_count, height, width = map(int, lines[1].split())

        imshow_sample(ax[i], finger, sample)
        mark_minutia(ax[i], finger, sample, minutiae[i], zoom=True)

        ax[i].set_title(f"Minutiae no. {minutiae[i]} for {image_name}", fontsize=20)
        ax[i].axis('off')
        ax[i].set_xlim([0, width])
        ax[i].set_ylim([height, 0])

if __name__ == '__main__':
    pass