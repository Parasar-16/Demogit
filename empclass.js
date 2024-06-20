class emp {
    _empname = "";

constructor (emname) 
{
this._empname = emname;
}
printName()
{
    console.log(this._empname);
}
}

var Emp = new emp ("Sowmya");
Emp.printName()