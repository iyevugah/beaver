lc = 0.4;

Point(1) = {-1.0, -0.5, -1.0, lc};
Point(2) = {+1.0, -0.5, -1.0, lc};
Point(3) = {+1.0, 0.0, -1.0, lc};
Point(4) = {+1.0, +0.5, -1.0, lc};
Point(5) = {-1.0, +0.5, -1.0, lc};
Point(6) = {-1.0, 0.0, -1.0, lc};
Point(7) = {-1.0, -0.5, +1.0, lc};
Point(8) = {+1.0, -0.5, +1.0, lc};
Point(9) = {+1.0, 0.0, +1.0, lc};
Point(10) = {+1.0, +0.5, +1.0, lc};
Point(11) = {-1.0, +0.5, +1.0, lc};
Point(12) = {-1.0, 0.0, +1.0, lc};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 1};
Line(7) = {3, 6};
Line Loop(1) = {1, 2, 7, 6};
Plane Surface(1) = {1};
Line Loop(2) = {-7, 3, 4, 5};
Plane Surface(2) = {2};
Line(8) = {7, 8};
Line(9) = {8, 9};
Line(10) = {9, 10};
Line(11) = {10, 11};
Line(12) = {11, 12};
Line(13) = {12, 7};
Line(14) = {9, 12};
Line Loop(3) = {8, 9, 14, 13};
Plane Surface(3) = {3};
Line Loop(4) = {-14, 10, 11, 12};
Plane Surface(4) = {4};
Line(15) = {1, 7};
Line(16) = {2, 8};
Line(17) = {3, 9};
Line(18) = {4, 10};
Line(19) = {5, 11};
Line(20) = {6, 12};
Line Loop(5) = {15, -13, -20, 6};
Plane Surface(5) = {5};
Line Loop(6) = {20, -12, -19, 5};
Plane Surface(6) = {6};
Line Loop(7) = {16, 9, -17, -2};
Plane Surface(7) = {7};
Line Loop(8) = {17, 10, -18, -3};
Plane Surface(8) = {8};
Line Loop(9) = {1, 16, -8, -15};
Plane Surface(9) = {9};
Line Loop(10) = {-4, 18, 11, -19};
Plane Surface(10) = {10};
Line Loop(11) = {-7, 17, 14, -20};
Plane Surface(11) = {11};

Surface Loop(1) = {1, 3, 5, 7, 9, 11};
Volume(1) = {1};
Surface Loop(2) = {2, 4, 6, 8, 10, 11};
Volume(2) = {2};

Physical Volume(1) = {1, 2};
Physical Surface("left") = {5, 6};
Physical Surface("right") = {7, 8};
Physical Surface("bottom") = {9};
Physical Surface("top") = {10};
Physical Surface("front") = {3, 4};
Physical Surface("back") = {1, 2};
Physical Surface("interface") = {11};