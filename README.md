# Orpheus
*A programming language with first-class LLM integration.*

Orpheus is a research project that aims to seamlessly integrate LLM operations into code. Programmers should be able to write declarative LLM-powered applications without the cruft of libraries like LangChain and Semantic Kernel.

There are three main goals for the language:
* First-class, model agnostic LLM operations (invocation, agents, tools, threads)
* Clean, readable syntax suited for mixing prompts and code
* Easy Python/Bash integration (Orpheus should function as a "glue" between models and other programs)

Currently, this repo contains an early prototype of the Orpheus interpreter written in Python. None of the LLM stuff has been implemented. That will come once the interpreter is rewritten in C/C++/Rust/whatever I decide.

# A tour of the interpreter
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
# LLM integration
*This is not yet implemented, but will be after the rewrite. This is what makes Orpheus actually useful.*
```
//Select model for program from config file:
model "gpt-3.5.conf"

//Create an agent, which has its own "thread" (conversation history) and can execute actions:
spawn myagent

//Store the result of an invocation to a variable:
var age: int? = << What is a good age to introduce a child to programming? >>
//Invocation will deduce the return type and either return an int or nil if there is a failure. Can be checked with `has(mynum).

//Bind an invokation to an agent:
myagent::<< Explain what programming is to a child that is {age} years old. >>
//This expression will resolve to a string, and the prompt/answer will be stored in the agent's thread.

//Use thread history from agent:
println(myagent::<< Summarize that explanation in a sentence. >>)

//Actions are functions that interact with the model, and can be bound to an agent.
// All invokations in an action share a thread, even if not bound to an agent.
action func fix_grammar(text: str) : str
->
	res: str? = << 
		Fix all of the grammar mistakes in {text}. 
		Return nothing if there are none.
	>>

	if has(str)
	->
		println(<< What type of mistake was most common? >>)
		return str
	else ->
		println("No grammatical mistakes in '" + text + "'.")
		return text

sentences: list = [
	"the quick brown fox jumping over the lazy dog",
	"the quick brown fox jumped over the lazy dog",
	"the kwick brown faux jumpd over the layzee dog",
	"the quick bron fox was jumped over the lazy dog"
]

fixed: list = []
var i: int = 0
while i < len(sentences)
->
	fixed.append(myagent::fix_grammar(sentences[i]))
	
println(myagent::<< Overall, what was the most common type of grammatical error? >>)

```



