class product {
constructor(product) {
this.product = product;
}

productID() {
console.log(`${this.product}  is placed`);
}
}

class order extends product {
constructor(order) {
super(product); 
this.order = order;// call the super class constructor and pass in the name parameter
}

orderID() 
{
console.log(`${this.order} is shipped .`);
}
}

var pro = new product('Mobile');
var ord = new order('Samsung');
pro.productID();
ord.orderID();


 	 	 
