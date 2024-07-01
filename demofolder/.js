const original = {a:1, b:{c:2}};
const copy = Object.assign({}, original);
copy.b.c = 3;
console.log(original.b.c);
