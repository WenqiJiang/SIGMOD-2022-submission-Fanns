import numpy as np

np.random.seed(123)
query_vec = np.random.randint(low=-100, high=100, size=128)
np.random.seed(456)
center_vec = np.random.randint(low=-100, high=100, size=128)

np.set_printoptions(threshold=100000000)

print("Query Vector: \n", list(query_vec))
print("Center Vector: \n", list(center_vec))

dist = query_vec - center_vec
dist_sqr = dist ** 2
print("Total distance: ", np.sum(dist_sqr))