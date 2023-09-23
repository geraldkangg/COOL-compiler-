
(*  Example cool program testing as many aspects of the code generator
    as possible.
 *)

class A {
num : Int;
value() : Int {
	num
};
};

class Main {
  main():Int {
    (new A).value()};
};

