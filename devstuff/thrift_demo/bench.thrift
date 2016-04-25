struct hi {
    1: string msg;
    2: i64 hello;
    5: binary data;
}

exception maybe {
    1: string why;
}

service echo {
    hi echo(1:hi msg) throws (1:maybe e);
}
