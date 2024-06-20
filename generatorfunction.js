function* idgenerator() {
    let id=0;
    while(true)
    {
        
        yield id++;
    }
}
const gene = idgenerator();
console.log(gene.next().value);
console.log(gene.next().value);
console.log(gene.next().value);