# JobRadius

A C++ console job search that does not stop when a district has too few results. It widens the search to neighboring districts one hop at a time, scores every job it finds, sorts them with a hand-written Merge Sort, and prints the best matches.

Built for the Data Structures and OOP project (Team DSCPP-III-2026-T243, Graphic Era, Dehradun).

## How it works
1. Enter a district, a skill, a minimum salary and a ranking mode.
2. Hash maps find the jobs in that district that need the skill.
3. If fewer than 3 are found, a breadth-first search visits neighboring districts (up to 3 hops), nearest first.
4. Each job is scored by the chosen ranking strategy and sorted, highest score first.
5. The top results are printed with their score and distance in hops.

## Project structure
| File | Module | What it holds | Data structure / OOP idea |
|---|---|---|---|
| `src/utils.h` | helpers | `toLower`, `trim`, `allDigits` | - |
| `src/Job.h`, `src/SampleData.h` | 1 Data | `Job` class, 15 sample jobs | Encapsulation |
| `src/JobIndex.h` | 2 Index | skill -> jobs, district -> jobs | Hash map |
| `src/DistrictGraph.h` | 3 Graph | district borders | Undirected, unweighted graph (adjacency list) |
| `src/Queue.h` | 4 Search (support) | hand-written `Queue` with `enqueue` / `dequeue` | Queue (first in, first out) |
| `src/JobSearch.h` | 4 Search | hash lookup + BFS expansion | BFS, hash set |
| `src/Ranking.h` | 5 Ranking | `RankingStrategy` and two strategies | Strategy pattern, inheritance, polymorphism |
| `src/Sorting.h` | 6 Sorting | stable Merge Sort | Merge Sort, O(n log n) |
| `src/main.cpp` | 7 Main | console menu | - |

## Build and run
```
g++ -std=c++17 -Wall -Wextra -pedantic -o jobradius src/main.cpp
./jobradius
```
or `make` (then `./jobradius`). On Windows use `jobradius.exe`.

## Tests
```
make test
```
or build each file in `tests/` the same way, for example `g++ -std=c++17 -o test_queue tests/test_queue.cpp`.

| Test file | Covers |
|---|---|
| `test_utils.cpp` | helper functions |
| `test_queue.cpp` | queue order, empty checks, reuse |
| `test_index.cpp` | job data and both hash indexes |
| `test_graph_search.cpp` | graph, BFS expansion, threshold and hop limit |
| `test_sorting.cpp` | Merge Sort: order, ties, empty and large input |
| `test_ranking.cpp` | both strategies, weights, salary, strategy swap, ranking with sorting |

## Ranking
- **Multi-factor:** `(0.5 x skill + 0.3 x proximity + 0.2 x salary) x 100`. Skill = matched skills / skills the job lists. Proximity = `1 / (1 + hops)`. Salary = `min(1.2, salary / minimum) / 1.2`, or 1 if no minimum.
- **Location-first:** `100 - 25 x hops` (not below 0), plus 10 if the salary meets the minimum, capped at 100.

## Example session
```
District: Dehradun   Skill: python   Salary: 0   Mode: 1
Rank #1 | 66/100 | 0 hop(s)  Software Engineer, Dehradun
Rank #2 | 66/100 | 0 hop(s)  Data Analyst, Dehradun
Rank #3 | 60/100 | 1 hop(s)  Python Developer, Haridwar
Districts searched: Dehradun -> Haridwar -> Tehri Garhwal -> Pauri Garhwal
```

## Limits
- 15 sample jobs across 7 Uttarakhand districts; district borders are written by hand.
- The threshold (3) and hop limit (3) are fixed in `src/main.cpp`.
- The skill part of the score divides by how many skills a job lists, so jobs with fewer listed skills score higher for the same match.
