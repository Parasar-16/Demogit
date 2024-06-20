<html>
<head>JAVASCRIPT ASSIGNMENT </head>
<body>
  <h1>Right click !
    goto Inspect => Console</h1>
<script>
function greet1() {
    return "Hello, Alice (Function Declaration)";
  }
  console.log(greet1());
  
  // Expression Function
  const greet2 = function () {
    return "Hello, Bob (Function Expression)";
  };
  console.log(greet2());
  
  // Arrow Function
  const greet3 = () => "Hello, Charlie (Arrow Function)";
  console.log(greet3());
  
  // Constructor Function
  const greet4 = new Function('return "Hello, Dave (Function Constructor)";');
  console.log(greet4());
  
  // Immediately Invoked Function Expression (IIFE)
  const greet5toEve = (function () {
    return "Hello, Eve (IIFE)";
  })();
  console.log(greet5toEve);
  
  // Generator Function
  function* Greet6() {
    yield "Hello, ";
    yield "World (Generator Function)";
  }
  const generator = Greet6();
  console.log(generator.next().value + generator.next().value);
  
  // Function returning a Function. I changed name to Yagna (team member)
  function returnGreet7() {
    return function (name) {
      return `Hey, ${name} (Returned Function)`;
    };
  }
  const greet7 = returnGreet7();
  console.log(greet7("FRANK"));
  
  // Object Method. I changed name to Harsha (Team Member)
  const greeter = {
    greetGRACE: function () {
      return "Hello, GRACE (Object Method)";
    }
  };
  console.log(greeter.greetGRACE());
  </script>
  </body>
  </html>