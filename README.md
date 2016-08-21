# Database Project
An in-memory database that supports queries across multiple keys and arbitrary ordering of results.

## Summary
I completed this project as a means to practice creating complex data structures (for this project I implemented a multimap BST class) as well as efficient searching and sorting algorithms. In order to properly implement the database system, I built a simple set of C++ classes:
- **MultiMap :**  A binary search tree that accepts multiple values for each key.
  
    ![image](https://cloud.githubusercontent.com/assets/15008279/17835963/27985d84-6737-11e6-99e5-1e511d25955b.png)
- **Database :**  A simple database that contains three primary data structures:
  - A schema description
  - A bunch of rows of data (aka data records)
  - One or more field indexes
    ![image](https://cloud.githubusercontent.com/assets/15008279/17835974/99dca1c0-6737-11e6-85b5-aeda9988b854.png)

A simple description of each class, what it is and how it works, is commented on their respective header files.

## Full Specs
The full project specs can be found [here](https://github.com/nehcney/Database-Project/blob/master/spec.doc).

-
Copyright (c) 2016 Yen Chen
