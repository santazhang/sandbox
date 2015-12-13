for (var a = 1; a <= 1000 / 3; a++) {
    for (var b = a; b <= 1000 / 2; b++) {
        var c = 1000 - a - b;
        if (a * a + b * b == c * c) {
            console.log(a, b, c);
            console.log(a * b * c);
        }
    }
}
