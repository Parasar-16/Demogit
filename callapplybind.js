var car = {
 registrationNumber : 'APO7DP9012',
  brand : "Benz"
}

function displayDetails(ownerName) {
console.log(ownerName +", this car is your : " +this.registrationNumber+ " " +this.brand);

}
displayDetails.apply(car,["Harsha"]) ;

displayDetails.call(car,"Phani");
