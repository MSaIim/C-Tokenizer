# ![](http://i.imgur.com/u1rZL91.png) C-Tokenizer
Split up a string into tokens. For example:

    ./tokenizer " array[xyz ] + = pi 3.14159e-10 "

    Word            "array"
    Left Brace      "["
    Word            "xyz"
    Right Brace     "]"
    Plus            "+"
    Equals          "="
    Word            "pi"
    Float           "3.14159e-10"
    
## ![](http://i.imgur.com/JOXezQB.png?) Usage
Compile with gcc or a compiler of your choice then run in terminal by passing a string.

    gcc -Wall tokenizer.c -o tokenizer
    ./tokenizer "string_here"

         mm
      /^(  )^\  
      \,(..),/      
        V~~V   
