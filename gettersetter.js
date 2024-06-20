class person {
    constructor (firstname, lastname){
        this.firstname = firstname;
        this.lastname = lastname;
    }
    get fullName(){
        return ` ${this.firstname} ${this.lastname} `
    }
    set fullName(name){
      const parts = name.spilt(' ');
      this.firstname = parts[0];
      this.lastname = parts[1];
    }
}
const Parasar = new person('jhon', 'Doe');
console.log(Parasar.fullName);

person.fullName = 'Harsha Phani';
console.log(person.fullName);