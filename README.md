# A tour of the Orpheus interpreter
The interpreter currently has functions, scoping, basic control flow, and a type system.

* Scope denoted by `->`.
* Types: `int`, `float`, `str`, `bool` `list`, `map`, `any`, `func`. 
	* Adding `?` to variable type allows it to be assigned `nil`.
	* `auto` functions as expected.
	* Types can be compared with `type()` or `is`
* Builtins: `len`, `print`, `println`, `tail`, `append`, `type`, `has`.
* Keywords: `and`, `or`, `not`, `true`, `false`, `if`, `else`,`var`, `return`, `pass`, `func`.
* All objects can be represented as strings.
* All variables are mutable.
* `list`s and `map`s can contain mixed types.

Examples:
```
func fizzbuzz(n: int)
->
    var x: auto = 1 // auto binds var to first type assigned

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
var people: list = [
    { "name": "Bob", "age": 25 },
    { "name": "Alice", "age": 17 },
    { "name": "John", "age": 22 }
]

func greet(person: map)
->    
    print(person["name"])
    print(" is ")
    print(person["age"])
    println(" years old.")


var i: int = 0
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
```
var test: any = 3.0 // any can contain any type
println(test)
test = "hello world!"
println(test)
```
```
if "hello" is str // "is" keyword allows type checking on any
->
    println("This will execute!")
```
```
func do_thing(data: any)
->
	if has(data) -> println(data)
	else -> println("No data.")
	
var mydata: str? = nil // ? denotes "maybe" type, which can be nil
do_thing(mydata) // outputs "No data."
mydata = "wow!"
do_thing(mydata) // outputs "wow!"
```



