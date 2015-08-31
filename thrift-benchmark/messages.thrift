struct Book {
    1: string title,
    2: string author,
    3: i64 pub_year
}

struct Column {
    1: string name,
    2: i32 value_type
}

struct Index {
    1: string key_column,
}

struct Table {
    1: string name,
    2: list<Column> columns,
    3: string primary_column,
    4: list<Index> index
}
