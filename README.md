# 2D Collision Detection
My code uses SFML, so the required lib / bin / dll files are necessary to run the code.

Here are my attempts at efficient collision detection algorithms using quadtrees and spatial hashing. My implementation of spatial hashing worked much better than my implementation of quadtrees. The difference between quadtree insert and quadtree no insert is that quadtree no insert was made with std::vector, while quadtree insert was made with pointers from one element to another. I wanted to design a more space-efficient algorithm for detecting collision while adding the ability to insert another element into the scene quickly. Big Balls.cpp is the application of spatial hashing to an object with two dimensions, not just a point.

<p align="center">
  <img src="https://user-images.githubusercontent.com/103447109/217917712-cb54927d-ecc2-4ddd-b656-f1c64f8523e9.png">
</p>
This shows my spatial hash program computing real-time collisions for 100,000 objects at about 60 fps.

<p align="center">
  <img src="https://user-images.githubusercontent.com/103447109/217918532-132654d4-9784-4c21-b80d-88766bbc0c45.png">
</p>
This shows my quadtree with insert computing collisions. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/103447109/217919391-80fd3632-a6f6-461c-bf1c-c0896eaa3bb7.png">
</p>

Bounding box for my quadtree no insert algorithm. The points inside are highlighted.
<p align="center">
  <img src="https://user-images.githubusercontent.com/103447109/217920491-f3921ad9-72c8-4013-83ca-5f562cd4d187.png">
</p>

Application of the spatial hash algorithm to two-dimensional circle collisions. The collision algorithm is rudimentary, but it shows that the spatial hash can be applied to objects with dimensions greater than one.
