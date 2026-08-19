### **General Challenges**

1.  **Skilled Tailors**
    *   **Description**: A shop has $N$ tailors, each with a skill level $s[i]$, and $M$ orders, each with a difficulty $d[j]$. A tailor $i$ can fulfill order $j$ only if their skill is greater than or equal to the order difficulty ($s[i] \ge d[j]$). Each order takes one hour to complete. The goal is to calculate the minimum number of hours ($T$) required for the tailors to fulfill all the orders.
    *   **Link**: This is a known problem on the [HackerEarth](https://www.hackerearth.com/practice/algorithms/searching/binary-search/practice-problems/algorithm/tailor-shop-7e77038e/) platform, often titled "Tailor Shop" or "Skilled Tailors."

2.  **Complicated Expression**
    *   **Description**: Given a sequence of $n$ integers and a window size $x$, you must calculate the result of an expression involving the sliding window. For each window $(a_i, a_{i+1}, \dots, a_{i+x-1})$, a function $fun$ returns the **rightmost number** with the **highest number of distinct prime factors**. The final task is to find the minimum value returned by $fun$ across all possible windows.
    *   **Link**: This was part of a specific HackerEarth Engineering Internship challenge. While not a standard public problem, similar variants can be found in HackerEarth's practice archives.

3.  **Maximizing Difference**
    *   **Description**: You are given a 1-indexed array $A$ of $n$ integers. You must find an index $i$ ($1 < i < n$) that maximizes the **absolute difference** between two counts: (1) the number of integers greater than $A[i]$ in the range $[1, i-1]$, and (2) the number of integers less than $A[i]$ in the range $[i+1, n]$.
    *   **Link**: Similar to "Complicated Expression," this was part of the [HackerEarth Engineering Internship](https://www.hackerearth.com/challenge/test/hackerearth-engineering-internship/) test.

4.  **New Company Name**
    *   **Description**: Given a list of $N$ existing company names, you must find the **lexicographically smallest** name for a new company. The restriction is that the new name must not contain any substring that is present in any of the existing company names.
    *   **Link**: This problem is typically associated with string manipulation and suffix structures on HackerEarth.

5.  **War Game**
    *   **Description**: On an $n \times n$ grid, a single $k$-decker ship (length $k$) is placed either horizontally or vertically. Some cells are marked as definitely empty ('#'), while others can contain part of the ship ('.'). You must find a cell that belongs to the maximum number of different possible ship locations and print that maximum count.
    *   **Link**: This is a classic grid-based counting problem, similar to problems found on [Codeforces](https://codeforces.com/problemset/problem/931/B) or HackerEarth.

---

### **CodeNation Test (February 3, 2018)**

6.  **Palindrome Query**
    *   **Description**: Given a string $str$ of size $n$, you must answer $q$ queries. For each query with bounds $l$ and $r$, you need to count how many palindromic substrings exist in the string such that their length is between $l$ and $r$ inclusive.
    *   **Link**: This specific version was used in the CodeNation recruitment test on HackerEarth.

7.  **Fill the Grid**
    *   **Description**: You are given a $3 \times n$ grid to be painted with three colors (A, B, and C). There are two restrictions: (1) All $n$ cells of a single row cannot have the same color, and (2) All 3 cells of a single column cannot have the same color. You must output the number of ways to paint the grid modulo $10^9 + 7$.
    *   **Link**: A common combinatorics/dynamic programming problem found in CodeNation's hiring challenges.

8.  **The k'th Vowel**
    *   **Description**: Starting with a string $p$, an intermediate string $s$ is created based on an array $info$. For each element in $info$, a prefix or suffix of $p$ is appended to $s$. Finally, for given indices in a $vowels$ array, you must return the vowel located at that position within string $s$.

---

### **InterviewBit / CN Campus Pool Test**

9.  **Longest Cycle in Permutation**
    *   **Description**: Given a permutation $P$ of the first $N$ positive integers, the goal is to change **at most 2 elements** to create a new permutation $S$ such that the longest cycle in $S$ is strictly shorter than the longest cycle in $P$. The output should be the size of the new shortest "longest cycle".
    *   **Link**: This problem is hosted on [InterviewBit](https://www.interviewbit.com/problems/permutation-cycles/) under their campus recruitment sections.

10. **Reduce to Unity**
    *   **Description**: Given a number $a$, you can perform one of three operations per step: (1) increment by 1, (2) decrement by 1, or (3) divide by one of its prime factors. The goal is to find the minimum number of steps to reduce $a$ to 1.
    *   **Link**: Frequently appears in InterviewBit's "CN Campus Pool Test."

11. **Game of Averages**
    *   **Description**: Given an array $A$ of $N$ integers and a target value $X$, you need to find the length $L$ of the **maximum size subarray** whose average is greater than or equal to $X$.
    *   **Link**: This is a variation of the "Longest Subarray with Average >= X" problem often found on platforms like InterviewBit and GFG.