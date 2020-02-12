response = await fetch('add.wasm');
bytes = await response.arrayBuffer();
exports = await WebAssembly.instantiate(bytes);
add = exports.instance.exports.my_add;
sub = exports.instance.exports.my_sub;


console.log(add(1,2));
console.log(add(4,5));
console.log(sub(7,2));
