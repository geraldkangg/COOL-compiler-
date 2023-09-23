
(*  Example cool program testing as many aspects of the code generator
    as possible.
 *)

class A {
num : Int;
set_val(n : Int) : SELF_TYPE {
  {
	num <- n;
  self;
  }
};
};

class Main {
  main():A {
    (let x : Int in
	 {
	    (new A).set_val(x);
	 }
      )
    };
};

