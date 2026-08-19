

Q1.

Puzzle.

Initially an unsorted array of N distinct number from 1 to N, in other words array of N natural numbers.

Ex N = 7, [1, 2, 3, 4, 6, 7, 5]

Some operation corrupts the array, 1 element replaced by another number from the range [1, N].

N = 7, [1, 2, 3, 4, 4, 7, 5]

Fix the array back.



Hint: 1 element get missed from range [1, N] in array, and 1 element will have double entries.


Q2.

SQL
You have table where cityid is getting mapped with multiple city names, fix the data
map cityid to most occuring cityname.

+------+-------+--------+
|cityid|hotelid|cityname|
+------+-------+--------+
|1     |101    |BLR     |
|1     |102    |BLR     |
|1     |103    |BLR     |
|1     |104    |BLR     |
|1     |105    |IT-BLR  |
|1     |106    |IT-BLR  |
|2     |201    |DEL     |
|2     |202    |DEL     |
|2     |203    |DEL     |
|2     |204    |DEL     |
|2     |205    |IT-DEL  |
|2     |206    |IT-DEL  |
+------+-------+--------+

Hint: can do some ranking operations


Q3.

Contructive Algorithms

Given a Seq of intervals, since intervals are not optimized, merge the overlapping intervals

Ex [ [1, 4], [3, 6], [10, 20] ] --> [ [1, 6], [10, 20] ]


Usecase was: optimizing checkins of hotels, setting up discount for multiple checkin - dates


If able to solve Q3
Q4.


Variation on Q3.

Given a Seq of intervals, the minimum number of intervals you need to remove to make
the rest of the intervals non-overlapping.

Ex [ [1, 2], [2, 3], [1, 3] ]
