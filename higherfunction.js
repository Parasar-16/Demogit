function WithLogging(fn){
 return function(...args){
 const result = fn(...args);
 console.log(`function ${fn.name} returned ${result}`);
 return result;
 };
}
const sum = (a ,b) => a+b;
const loggedSum = WithLogging(sum);
loggedSum(4,5);