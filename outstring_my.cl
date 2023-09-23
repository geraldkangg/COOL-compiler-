class Main inherits IO {
    x : Int <- 3;
    y : Int <- 2;
   
    main() : Object {
      {
            out_string("testing lt: \n");
            if x < y then out_string("x less than y \n") else out_string("y less x\n") fi;
      }
    };
};