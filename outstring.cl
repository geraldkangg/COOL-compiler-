class Main inherits IO {
    x : Bool <- true;
    y : String <- "true";
   
    main() : Object {
        {
            out_string("testing case: \n");
            case x of  
                a : Bool => out_string("matched bool \n");
                b : Object => out_string("matched object \n");
                c : String => out_string("matched string \n");
            esac;    

            case y of  
                a : Bool => out_string("matched bool \n");
                b : Object => out_string("matched object \n");
                c : String => out_string("matched string \n");
            esac;    
        }
    };
};