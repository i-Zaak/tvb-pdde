Point(1) = {0, 100, -0, 5};
Point(2) = {0, 0, -0, 5};
Point(3) = {100, 0, -0, 5};
Point(4) = {100, 100, -0, 5};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line Loop(5) = {4, 1, 2, 3};
Plane Surface(6) = {5};
