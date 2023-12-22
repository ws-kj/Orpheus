# Orpheus
An AI-first programming language

--

This is a research prototype.

Core ideas:
*   First class LLM operations (invocation, agents, vectorization, etc)
*   Clean, minimal syntax suited for mixing prompts and code
*   Seamless Python interop

Development goals:
*    Python based interpreter (dynamic, no LLM)
*    Add static typing
*    Add LLM integration
*    Add python interop
*    Rewrite in C/C++/Rust

--

Examples:

```
func fizzbuzz(n)
->
    var x = 1

    while x <= n
    ->
        if x % 3 == 0 and x % 5 == 0 -> println("FizzBuzz")
        else if x % 3 == 0 -> println("Fizz")
        else if x % 5 == 0 -> println("Buzz")
        else -> println(x)
        
        x = x + 1

fizzbuzz(20)
```

```
var people = [
    { "name": "Bob", "age": 25 },
    { "name": "Alice", "age": 17 },
    { "name": "John", "age": 22 }
]

func greet(person)
->    
    print(person["name"])
    print(" is ")
    print(person["age"])
    println(" years old.")


var i = 0
while i < len(people)
->
    greet(people[i])
    if people[i]["age"] < 21
    ->
        println("Not eligible to drink!")
    else 
    ->
        println("Eligible to drink.")
    println()

    i = i + 1
```

:)
