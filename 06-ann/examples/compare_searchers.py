from ball_tree_example import main_balltree
from kd_tree_example import main_kdtree
from naive_search_example import main_naive
from annoy_example import main_annoy
from voyager_example import main_voyager

N = 100000
Q = 300
d = 128
k = 15

def main_compare():
    #main_naive(N, Q, d, k)
    #main_kdtree(N, Q, d, k)
    main_balltree(N, Q, d, k)
    main_annoy(N, Q, d, k)
    main_voyager(N, Q, d, k)

if __name__ == "__main__":
    main_compare()