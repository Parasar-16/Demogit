
const str1 ="Hello world !";
const str2 = 'Hello Excel R';
const str3 = ` Welcome to JavaScrpit`;
const string = str1 + str2 + str3 ;  // concatinating the strings 
console.log(string);

const age = 30; // template literals
const text = `my age is ${age} years old. `;
console.log(text); 

// Acessing the string characters
const str4 = "Harsha phani Vardhan Parasar";
const char = str4[5];
console.log(char);
const length = str4.length;
console.log(length);



//string methods


const Name = "Marepalli Harsha Phani Vardhan Parasar";
const upper = Name.toUpperCase();
const lower =Name.toLowerCase();
const Slice = Name.slice(0, 7);
console.log(upper);
console.log(lower);
console.log(Slice);

const hello123 = "Hello world!";
const index = hello123.indexOf('world');
console.log(index);

const hey123 = "Hello world!";
const includesworld = hey123.includes('world');
console.log(includesworld);

const str5 = "hello world!";
const newstr = str5.replace("hello", "everyone!");
console.log(newstr);


