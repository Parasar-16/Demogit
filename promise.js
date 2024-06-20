const count = true;

let countValue = new Promise(function (reslove , reject){
 if(count){
    reslove("I have completed the all operations.");
 }
 else{
    reject("There is no count value");
 }
});
console.log (countValue);
