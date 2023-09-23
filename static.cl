
(*  Example cool program testing as many aspects of the code generator
    as possible.
 *)

class A {
num : Int;
value() : Int {
	num
};
};

class B inherits A {
  bool : Bool;
};

class Main {
  main():Int {
    (new B)@A.value()};
};

