var upper = 2 * 1000 * 1000;

var is_prime = [];

is_prime[0] = false;
is_prime[1] = false;

for (var i = 2; i <= upper; i++) {
    is_prime[i] = true;
}

var sum = 0;
for (var i = 2; i <= upper; i++) {
    if (is_prime[i]) {
        console.log(i);
        sum += i;
        for (var j = i; j <= upper; j += i) {
            is_prime[j] = false;
        }
    }
}

console.log("sum", sum);
