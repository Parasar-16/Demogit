class User{
_name ="";
constructor(Name)
{
    this._name= Name;
}
sayHi()
{
    console.log(this._name);
}
}
var user = new User("Harsha");
user.sayHi();