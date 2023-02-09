# Quadtree
My code uses SFML, so the required lib / bin / dll files are necessary to run the code.

Here are my attempts at efficient collision detection algorithms using quadtrees and spatial hashing. My implementation of spatial hashing worked much better than my implementation of quadtrees. The difference between quadtree insert and quadtree no insert is that quadtree no insert was made with std::vector, while quadtree insert was made with pointers from one element to another. I wanted to design a more space efficient algorithm for detecting collision while also adding the ability to quickly insert another element into the scene.
