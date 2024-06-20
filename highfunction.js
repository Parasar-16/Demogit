const greet =  function(name){ 
    return function(message){ 
    
        console.log(`Hi!! ${name}, ${message}`); 
    } 
} 
  
const greet_message = greet('Harsha'); 
greet_message("Welcome To universe")