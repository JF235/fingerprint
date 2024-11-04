from ball_tree_example import main_balltree
from kd_tree_example import main_kdtree
from naive_search_example import main_naive

N = 100000
Q = 40
d = 128
k = 3

def main_compare():
    main_naive(N, Q, d, k)
    main_kdtree(N, Q, d, k)
    main_balltree(N, Q, d, k)

if __name__ == "__main__":
    main_compare()