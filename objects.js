const person = {
    firstNamme: "Harsha",
    lastName: "Marepalli",
    age: 30
};
console.log(person.firstNamme); //type:1 dispalying output

console.log(person.lastName);

console.log(person.age);
console.log(person['firstNamme']); //type:2 dispalying output


console.log(person['lastName']);

console.log(person['age']);

person.job ="enginner"; //adding new  property
person.age = 31;      

console.log(person.age);

console.log(person.job)

delete person.job;    //deleting New property
console.log(person.job);

//Acess the property of the Nested Property

const person1 ={
    name: {
        first : 'Harsha',
        last : 'Phani'
    },
    age : 22
};
console.log(person1.name.first);

