const countwords = (str) => {
    const words = str.split(' ');
let count = 0 ;
 for(let i = 0; i< words.length; i++){
    if (words[i] != 0){
        count++ ;
    }
 }
 return count;
};

const str1 = "Marepalli Harsha Phani vardhan Parasar"

console.log(`the number of words in ${str1} is ${countwords(str1)}`);

const sentence = "harsha phani";
const letters = sentence.split('');
console.log(letters.length);

let num = [1 , 2 , 3 , 4 , 5 , 6, 7,  8, 9, 10]
let find = num.indexOf(4);
console.log(find)

const spell = "harsha"
const yes = spell.includes('a')
console.log(yes);
