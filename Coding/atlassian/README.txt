
https://codewitharyan.com/system-design/low-level-design

https://leetcode.com/discuss/post/6974858/disclosed-atlassian-full-loop-question-b-ulje/
https://leetcode.com/discuss/post/7070747/atlassian-p40-india-offer-by-anonymous_u-6mbf/
https://leetcode.com/discuss/post/7246118/atlassian-p60-sept-2025-rejected-by-anon-m7ye/
https://leetcode.com/discuss/post/1976694/atlassian-onsite-api-to-get-monthlyannua-y8f2/
https://leetcode.com/discuss/post/6938816/atlassian-senior-software-engineer-p50-o-4hf6/
https://leetcode.com/discuss/post/6959323/atlassian-p40-india-offer-by-anonymous_u-vldq/

https://leetcode.com/problems/stock-price-fluctuation/description/
https://leetcode.com/problems/text-justification/description/

Current hot LLD question at Atlassian: Design a customer support agent rating system.
Majorly they are asking to design the Jira Board / Feature flag


Karate Round -> 5 system design question, word search problem
Coding Round -> Employee Hirarchy
Code Design Round -> Rating System, get aggregated rating
System Design -> Similar to twitter feed
Managerial Rounds -> Normal Managerial questions
Values Round -> Almost same as Managerial Rounds but around atlassian core values

System Design for a notion like a note-taking app.
Design a Tagging system for Jira. Discuss: the API, the data structure, scaling it up, how would you generate daily "trending tags".
Design a webpage where you will show a lot of documents as a list?
Design JIRA Kanban Board view (The question was to design a Jira bulletin board (Design the UI/UX experience of a Jira board (personal and team)).
I was led to create a general design of a personal board. Then we focused on the design of a team board. We then narrowed in on the design of individual cards. 
He asked how I would lay out title, subtitle, contributors, etc. I went on a brief tangent on how the DB schema would look, but I quickly caught on that the interviewer was strictly focused on UI/UX. I created a good layout and explained my reasoning, and everything was going great until I was asked about the debug process. As a full-stack engineer, there is so much that could go wrong. In particular, the interviewer asked how I would go about debugging empty cards. I asked follow-up questions to see if this was a networking issue, server, or client concern. He was urging me to just throw out ideas. So I did. Incorrect access controls, networking certificate issues, opening dev tools to check errors, and client/server TLS mismatch. Once I exhausted my list of possible sources of issues, he explained he expected me to open React dev tools and peek at the virtual DOM. I guess I over-engineered my debug process, but to be fair, I did ask for further clarification, and I wasn't given any. )

Problem Description
In a town with n houses aligned in a straight line, numbered from 1 to n from left to right. A virus is spreading from an initially infected house. Every day an infected house spreads the virus to its immediate uninfected neighbors.

Specifically, if house number X is infected on day i, then houses X-1 and X+1 will become infected on day i+1 if they are not already infected. Eventually, all houses will become infected. The sequence in which the houses get infected is called the infection sequence.

Given integer n and an integer array infectedHouses representing the initial infected houses, determine the total number of distinct infection sequences possible, modulo (109 + 7).

Examples
Example 1:
Input: n = 5, infectedHouses = [1, 5]
Output: 2
Explanation: Initially, houses 1 and 5 are infected. The infection progresses as follows:

On Day 1, both houses numbers 2 and 4 become infected.
On Day 2, house number 3 is infected. Now all the houses are infected.
There is no way that house number 3 can be infected before houses 2 and 4. The 2 possible infection sequences are [2,4,3] and [4,2,3].

Example 2:
Input: n = 6, infectedHouses = [3, 5]
Output: 6
Explanation: Initially, houses 3 and 5 are infected. The houses look like: [1,2,3,4,5,6].

On Day 1, houses number 2, 4, 6 get infected. The houses look like this: [1,2,3,4,5,6].
On Day 2, house number 1 gets infected. All the houses are infected now.
The 6 possible infection sequences are: [2,4,6,1], [2,6,4,1], [4,2,6,1], [4,6,2,1], [6,2,4,1], [6,4,2,1].

Example 3:
Input: n = 4, infectedHouses = [1]
Output: 1
Explanation: Initially, house 1 is infected. The houses look like: [1,2,3,4].

On Day 1, house number 2 gets infected. The houses look like this: [1,2,3,4].
On Day 2, house number 3 gets infected. The houses look like this: [1,2,3,4].
On Day 3, house number 4 gets infected. All houses are infected now.
The only possible infection sequence is [2,3,4].

Constraints
2 ≤ n ≤ 105
1 ≤ m ≤ n-1, where m is the length of infectedHouses.
1 ≤ infectedHouses[i] ≤ n
All elements of the array are distinct.